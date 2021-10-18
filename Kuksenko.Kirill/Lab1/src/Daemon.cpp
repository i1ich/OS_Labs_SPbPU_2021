#include "Daemon.h"

#include <cstring>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>

std::vector<std::string> const DaemonGrammar::tokens = {"dir_in", "dir_out", "time"};

Daemon* Daemon::inst = nullptr;
std::string const Daemon::pid_file = std::filesystem::absolute("pid.pid");
bool Daemon::terminate = false;
bool Daemon::reread = false;

void Daemon::signal_handler(int signal_id) {
    switch (signal_id)
    {
    case SIGHUP:
        reread = true;
        break;

    case SIGTERM:
        terminate = true;
        break;
    
    default:
        break;
    }
}

Daemon* Daemon::instance(std::string const& config) {
    if (!inst) {
        inst = new Daemon(config);
    }

    return inst;
}

Daemon::Daemon(std::string const& config) : grammar(DaemonGrammar()), cfg{ std::filesystem::absolute(config) }, logger{ Logger("Daemom") } {
    reread_congig();
}

Daemon::~Daemon() {
    logger.log(Logger::INFO, "daemon is terminated");
}

int Daemon::reread_congig() {
    ConfigParser parser(cfg, grammar);
    parse_tokens(parser.parse());

    return 0;
}

void Daemon::parse_tokens(std::vector<ConfigParser::Token> const& tokens) {
    for (auto const& elem : tokens) {
        if (elem.token == grammar.get_token(TOKENS::DIR_IN)) {
            dir_in = std::filesystem::absolute(elem.value);
        }
        else if (elem.token == grammar.get_token(TOKENS::DIR_OUT)) {
            dir_out = std::filesystem::absolute(elem.value);
        }
        else if (elem.token == grammar.get_token(TOKENS::REPEAT_SIZE)) {
            repeat_time = std::stoi(elem.value);
        }
        else {
            logger.log(Logger::WARNING, "unknown config token");
        }
    }
}

int Daemon::run() {
    logger.log(Logger::INFO, "daemon is runnig");

    pid_t pid = executing_pid();

    if (pid == -2) {
        return EXIT_FAILURE;
    }
    if (pid > 0) {
        kill(pid, SIGTERM);
    }

    init();

    if (create_pid_file() == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    setup_handler();

    logger.log(Logger::INFO, "daemon is started to work");

    while (!terminate) {
        if (reread) {
            reread_congig();
            reread = false;
        }

        if (work() == EXIT_FAILURE) {
            return EXIT_FAILURE;
        }

        sleep(repeat_time);
    }

    logger.log(Logger::INFO, "daemon is finished work successfully");
    return EXIT_SUCCESS;
}

pid_t Daemon::executing_pid() const {
    if (!std::filesystem::exists(pid_file)) {
        return -1;
    }

    std::ifstream in_pid(pid_file);

    if (!in_pid.is_open()) {
        logger.log(Logger::ERROR, "can't open pid file");
        return -2;
    }

    pid_t pid;

    in_pid >> pid;

    struct stat sts;

    if (stat((std::string("/proc/") + std::to_string(pid)).c_str(), &sts) == -1 && errno == ENOENT) {
        pid = -1;
    }

    in_pid.close();

    return pid;
}

int Daemon::create_pid_file() const {
    std::ofstream out_pid(pid_file);

    if (!out_pid.is_open()) {
        logger.log(Logger::ERROR, "can't create pid file");
        return EXIT_FAILURE;
    }

    out_pid << getpid() << std::endl;

    out_pid.close();

    return EXIT_SUCCESS;
}

int Daemon::init() {
    pid_t pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    if (setsid() < 0) {
        exit(EXIT_FAILURE);
    }

    if (chdir("/") < 0) {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    return 0;
}

void Daemon::setup_handler() const {
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = signal_handler;
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGHUP);
    sigaddset(&sigset, SIGTERM);
    act.sa_mask = sigset;
    sigaction(SIGHUP, &act, nullptr);
    sigaction(SIGTERM, &act, nullptr);
}

int Daemon::work() {
    std::ofstream total_out(dir_out + std::string("/total.log"), std::ios::out | std::ios::app);

    if (!total_out.is_open()) {
        logger.log(Logger::ERROR, "can't open total file");
        return EXIT_FAILURE;
    }

    for (auto& p: std::filesystem::directory_iterator(dir_in)) {
        auto path = p.path();

        if (path.extension() == extension) {
            char* data = file_content(path);

            if (!data) {
                return EXIT_FAILURE;
            }

            if (!first_line) {
                total_out << "\n\n";
            }
            if (first_line) {
                first_line = false;
            }

            total_out << path << "\n\n" << data << std::endl;

            remove(path);
            delete[] data;
        }
    }

    total_out.close();

    return 0;
}

char* Daemon::file_content(std::string const& filename) const {
    FILE* file_in = fopen(filename.c_str(), "r");

    if (!file_in) {
        logger.log(Logger::ERROR, "can't open file for read");
        return nullptr;
    }

    fseek(file_in, 0, SEEK_END);
    size_t file_size = ftell(file_in);
    fseek(file_in, 0, SEEK_SET);
    char* file_data = new char[file_size];
    
    if (!file_data) {
        fclose(file_in);

        logger.log(Logger::ERROR, "memory allocation error");
        return nullptr;
    }

    size_t readed_size = fread(file_data, sizeof(char), file_size, file_in);
    file_data[readed_size] = 0;

    fclose(file_in);

    return file_data;
}
