//
// Created by oleg on 22.10.2021.
//

#include "../includes/Daemon.h"

#include <utility>
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

const char SPACE = ' ';
const int SLEEPTIME = 30;

Daemon Daemon::instance;

Daemon Daemon::getInstance(std::string configPath) {
    instance.configPath = std::move(configPath);
    return instance;
};

bool Daemon::init() {
    openlog("MY_DAEMON", LOG_NDELAY | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "INFO: Start initializing daemon");

    runDaemon = true;
    readAgain = false;
    pidPath = Daemon::getFullWorkingDirectory(PID_FILE);
    syslog(LOG_INFO, "%s", Daemon::getFullWorkingDirectory(PID_FILE).c_str());
    _parser.setConfig(configPath);

    if (!_parser.parse()) {
        stopDaemon();
        syslog(LOG_ERR, "ERROR: Can't parse config");
        return false;
    }

    pid_t pid = fork();
    if(!checkPid(pid)){
        return false;
    }
    umask(0);
    if (setsid() < 0) {
        stopDaemon();
        syslog(LOG_ERR, "ERROR: Can't create session");
        return false;
    }

    if (chdir("/") == -1) {
        stopDaemon();
        syslog(LOG_ERR, "ERROR: Can't change working directory");
        return false;
    }

    for (int i = sysconf(_SC_OPEN_MAX); i > 0; i--) {
        close(i);
    }

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    if (!setPidFile()) {
        syslog(LOG_ERR, "ERROR: In create pid file");
    }

    return true;
}

bool Daemon::checkPid(pid_t pid) {
    switch (pid) {
        case -1:
            stopDaemon();
            syslog(LOG_ERR, "ERROR: Can't create child");
            return false;
        case 0:
            syslog(LOG_INFO, "INFO: Create child process");
            return true;
        default:
            //kill(pid, SIGTERM);
            syslog(LOG_INFO, "INFO: Kill parent process");
            return false;
    }
}

std::string Daemon::getFullWorkingDirectory(const std::string &path) {
    if (path.empty() || path[0] == '/') {
        return path;
    }
    return std::filesystem::current_path().string() + "/" + path;
}

void Daemon::signal_handler(int signal_id) {
    switch (signal_id) {
        case SIGHUP:
            instance.readAgain = true;
            break;
        case SIGTERM:
            instance.stopDaemon();
            break;
        default:
            break;
    }
}

bool Daemon::setPidFile() {
    std::ofstream out(pidPath);
    if (!out.is_open()) {
        auto pid = getpid();
        if (!pid) {
            stopDaemon();
            syslog(LOG_ERR, "ERROR: Can't get pid\n");
            return false;
        }
        out << getpid() << std::endl;
        out.close();
        return true;
    }
    stopDaemon();
    syslog(LOG_ERR, "ERROR: Can't create pid file\n");
    return false;
}

void Daemon::run() {
    std::pair<std::string, int> record;
    while (runDaemon && _parser.getPath(record)) {
        work(record);
        sleep(SLEEPTIME);
        if (readAgain) {
            _parser.parse();
            readAgain = false;
        }
    }
    syslog(LOG_INFO, "INFO: Daemon end work\n");
    closelog();
}

void Daemon::work(std::pair<std::string, int> record) {
    std::string path = record.first;
    int depth = record.second;
    std::filesystem::path startPath(path);
    if (startPath.empty()) {
        syslog(LOG_ERR, "ERROR: Can't find path:{%s}", path.c_str());
    }
    recursiveDelete(startPath, depth - 1);
}

void Daemon::recursiveDelete(const std::filesystem::path &path, int depth) {
    if (depth == 0) {
        for (auto &p: std::filesystem::directory_iterator(path)) {
            if(Daemon::isDirectory(p.path().string())) {
                std::filesystem::remove(p);
            }
        }
    } else {
        for (auto &p: std::filesystem::directory_iterator(path)) {
            recursiveDelete(p.path(), depth - 1);
        }
    }
}

bool Daemon::isDirectory(const std::string& path) {
    struct stat s{};
    if (stat(path.c_str(), &s) == 0) {
        if (s.st_mode & S_IFDIR) {
            return true;
        }
    }
    return false;
}

void Daemon::stopDaemon() {
    runDaemon = false;
    syslog(LOG_INFO, "INFO: daemon release");
}
