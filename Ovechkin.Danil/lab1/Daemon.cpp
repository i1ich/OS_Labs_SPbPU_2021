#include "Daemon.h"


Daemon Daemon::instance;

Daemon& Daemon::getInstance() {
    return Daemon::instance;
}

void Daemon::init(std::string const&configFileName) {

    openlog("DAEMON_6", LOG_PID | LOG_NDELAY, LOG_USER);
    syslog(LOG_INFO, "Daemon initialization is starting");

    isWorking = true;

    char buf[FILENAME_MAX];

    if (!getcwd(buf, FILENAME_MAX)) {
        syslog(LOG_ERR, "Failed to get home path");
        stop();

        throw std::runtime_error("Failed to get home path");
    }


    homePath = buf;
    fullConfigPath = convertToPath(configFileName);

    if (!setConfigParams()) {
        stop();
        throw std::runtime_error("Problems with reading config");
    }

    pid_t pid = fork();

    if (pid == -1) {
        syslog(LOG_ERR, "Fork failed");
        stop();

        throw std::runtime_error("Fork failed");
    } else if (pid != 0) {
        syslog(LOG_INFO, "Now in the parent process");
    }

    syslog(LOG_INFO, "First fork success");

    pid = fork();

    if (pid == -1) {
        syslog(LOG_ERR, "Fork failed");
        stop();
        throw std::runtime_error("Fork failed");
    } else if (pid != 0) {
        syslog(LOG_INFO, "Now in the parent process");
    }

    syslog(LOG_INFO, "Second fork success");

    umask(0);

    if (chdir("/") == -1) {
        syslog(LOG_ERR, "chdir() failed");
        stop();

        throw std::runtime_error("chdir() failed");
    }

    if (close(STDIN_FILENO) == -1 || close(STDOUT_FILENO) == -1 || close(STDERR_FILENO) == -1) {
        syslog(LOG_ERR, "Failed while closing");
        stop();

        throw std::runtime_error("Failed while closing");
    }

    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);


    if (!handlePidFile()) {
        stop();
        throw std::runtime_error("Problems with Pid");
    }

}

std::string Daemon::convertToPath(const std::string &configFileName) {

    std::string path = homePath + "/" + configFileName;

    char* realPath = realpath(path.c_str(), nullptr);

    if (!realPath) {
        syslog(LOG_ERR, "Problems with path");

        return std::string();
    }

    return std::string(realPath);
}

void Daemon::signalHandler(int signal) {
    Daemon& daemon = Daemon::getInstance();

    switch (signal) {
        case SIGHUP:
            if (!daemon.setConfigParams()) {
                daemon.stop();
            }
            syslog(LOG_INFO, "Config is updated");
            break;
        case SIGTERM:
            daemon.stop();
            break;
        default:
            syslog(LOG_INFO, "Can't handle signal %i", signal);
    }

}

bool Daemon::handlePidFile() {
    syslog(LOG_INFO, "Start handling PID file");

    std::ifstream pidFile(pidFilePath);

    if (!pidFile.is_open()) {
        syslog(LOG_INFO, "Pid file couldn't be opened");
    } else if (!pidFile.eof()) {
        pid_t oldPid;

        pidFile >> oldPid;
        pidFile.close();

        struct stat st;
        std::string oldPath = "/proc/" + std::to_string(oldPid);

        if (stat(oldPath.c_str(), &st) == 0)
            kill(oldPid, SIGTERM);
    }

    return setPidFile();
}

bool Daemon::setPidFile() {
    std::ofstream pidFile(pidFilePath);
    if (!pidFile.is_open()) {
        syslog(LOG_ERR, "Pid file couldn't be opened");
        return false;
    }

    pidFile << getpid();
    pidFile.close();

    return true;
}

bool Daemon::setConfigParams() {

    syslog(LOG_INFO, "Start getting config params");

    ConfigParser& parser = ConfigParser::getInstance();

    try {
        parser.parse(fullConfigPath);
        std::map<ConfigParser::ConfigParams, std::string> params = parser.getParams();

        dir1 = params.at(ConfigParser::DIR1);
        time = std::stoi(params.at(ConfigParser::TIME));

    } catch (std::runtime_error &error) {
        return false;
    }

    return true;
}


void Daemon::stop() {

    isWorking = false;
    syslog(LOG_INFO, "Deamon is stopped");
    closelog();
}


void Daemon::run() {
    syslog(LOG_INFO, "Process is running");

    while (isWorking && doTask()) {
        sleep(time);
    }
}

bool Daemon::doTask() {
    std::string fullpath = homePath + "/" + dir1;

    for (auto& path: std::filesystem::directory_iterator(fullpath)) {
        if (path.is_directory()){
            std::filesystem::remove_all(path);
            syslog(LOG_INFO, "Directory was deleted");
        }
    }

    return true;
}


