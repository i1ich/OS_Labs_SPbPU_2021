#include "DaemonManager.h"
#include "Daemon.h"

#include <signal.h>
#include <syslog.h>
#include <unistd.h>

#include <exception>
#include <string>

const char* DaemonManager::_pidFilename = "/var/run/kirpichenko_daemon/.pid";
std::string DaemonManager::_cfgPath;
Daemon* DaemonManager::_daemon = nullptr;

void DaemonManager::writePID(std::ofstream& pidFile) {
    if (!pidFile.good()) {
        std::string msg("Unable to write a pid file (");
        msg.append(_pidFilename);
        msg.append(")!");
        throw std::runtime_error(msg);
    }
    pidFile << getpid();
}

int DaemonManager::readPID(std::ifstream& pidFile) {
    int pid = -1;
    if (pidFile.good()) {
        pidFile >> pid;
    }
    return pid;
}

void DaemonManager::findConfig() {
    std::filesystem::directory_iterator iter(std::filesystem::current_path());
    for (auto& content: iter) {
        if (content.path().extension() == ".cfg") {
            _cfgPath = content.path();
            return;
        }
    }
    throw std::runtime_error("Unable to find .cfg file");
}

void DaemonManager::createNewSession() {
    std::ifstream pidFileIn(_pidFilename, std::ios_base::in);
    pid_t pid = readPID(pidFileIn);
    if (pid > 0 && kill(pid, 0) == 0) {  // process with this pid exists
        if (kill(pid, SIGTERM) == -1) {
            throw std::runtime_error("Unable to kill already running daemon!");
        }
        syslog(LOG_INFO, "Previous daemon with pid %i was killed", pid);
    }
    pidFileIn.close();
    std::ofstream pidFileOut(_pidFilename, std::ios_base::out | std::ios_base::trunc);
    writePID(pidFileOut);
    pidFileOut.close();
    if (_cfgPath.empty()) {
        findConfig();
    }
    if (chdir("/") == -1) {
        syslog(LOG_WARNING, "Unable to change working directory");
    }
    if (signal(SIGTERM, DaemonManager::signalHandler) == SIG_ERR ||
        signal(SIGHUP, DaemonManager::signalHandler) == SIG_ERR) {
        throw std::runtime_error("Unable to set signals handler");
    }
    _daemon = Daemon::getDaemon();
    _daemon->proceed();
}

const char* DaemonManager::getConfigPath() {
    return _cfgPath.c_str();
}

void DaemonManager::signalHandler(int signal) {
    switch (signal) {
        case SIGHUP:
        _daemon->queryConfigReload();
        syslog(LOG_INFO, "Loading new config file");
        break;
        case SIGTERM:
        _daemon->stop();
        syslog(LOG_INFO, "Daemon ends its work due to SIGTERM signal");
        break;
    }
}