//
// Created by pikabol88 on 10/20/21.
//

#include "Daemon.h"

Daemon Daemon::_instance;

Daemon &Daemon::getInstance() {
    return _instance;
}

bool Daemon::init(const std::string &config) {
    _pidFileName = "/var/run/lab1.pid";
    openlog("daemon", LOG_PID|LOG_NDELAY, LOG_USER);
    syslog(LOG_INFO, "Initializing daemon");

    try {
        char buf[FILENAME_MAX];
        if (!getcwd(buf, FILENAME_MAX)) {
            throw std::runtime_error("Failed in getcwd");
        }
        _homeDir = buf;
        syslog(LOG_INFO, "Dir home - %s", buf);
        if (!initTread()) {
            return false;
        }
        initSignals();
        setConfig(config);
        loadConfig();

        syslog(LOG_INFO, "Daemon is successfully initialized");
        _isRunning = true;

    } catch(std::exception const& e) {
        syslog(LOG_ERR, "ERROR:: %s", e.what());
        Daemon::getInstance().terminate();
        return false;
    }

    return true;
}

void Daemon::run() {
    syslog(LOG_INFO, "Start running daemon");

    while (_isRunning) {
        moveAllFiles(_dir1, _dir2);
        removeAllFilesFromDir(_dir1);
        sleep(_delayTimeInterval);
    }
}

void Daemon::terminate() {
    _isRunning = false;
    unlink(_pidFileName.c_str());
    closelog();
}

bool Daemon::initTread() {
    syslog(LOG_INFO, "Start init thread");
    pid_t pid_t = fork();
    if (pid_t == -1) {
        throw std::runtime_error("Fork failed");
    } else if (pid_t == 0) {
        return initPid();
    }
    return false;
}

bool Daemon::initPid(){
    if (setsid() == -1) {
        throw std::runtime_error("Setsid return error");
    }
    pid_t pid = fork();
    if (pid == -1) {
        throw std::runtime_error("Fork failed");
    }
    if(pid != 0){
        return false;
    }
    umask(0);
    if (chdir("/") == -1) {
        throw std::runtime_error("Chdir return error: %d");
    }

    if (close(STDIN_FILENO) == -1 || close(STDOUT_FILENO) == -1 || close(STDERR_FILENO) == -1) {
        throw std::runtime_error("Close return error: %d");
    }

    checkPid();

    return true;
}

void Daemon::checkPid() {
    syslog(LOG_INFO, "Handle PID file");
    std::ifstream pidFile(Daemon::_pidFileName);
    if (!pidFile.is_open()) {
        throw std::runtime_error("Can't open pid file");
    }
    pid_t other;
    pidFile >> other;
    syslog(LOG_INFO, "Close PID file");
    pidFile.close();

    struct stat sb;
    std::string path = "/proc/" + std::to_string(other);
    if (stat(path.c_str(), &sb) == 0) {
        kill(other, SIGTERM);
    }
    savePid();
}

void Daemon::savePid() {
    syslog(LOG_INFO, "Save PID file");
    std::ofstream out(Daemon::_pidFileName);
    if (!out.is_open()) {
        throw std::runtime_error("Can't open pid file");
    }
    out << getpid();
    out.close();
    syslog(LOG_NOTICE, "Thread init complete");
}

void Daemon::initSignals() {
    syslog(LOG_INFO, "Signals initialization");

    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);
}


void Daemon::signalHandler(int signalNum) {
    syslog(LOG_INFO, "Handle signal %i", signalNum);
    switch (signalNum) {
        case SIGHUP:
            syslog(LOG_INFO, "Updating config file");
            Daemon::getInstance().loadConfig();
            break;
        case SIGTERM:
            syslog(LOG_INFO, "Terminate daemon");
            Daemon::getInstance().terminate();
            break;
        default:
            syslog(LOG_INFO, "Signal %i is not handled", signalNum);
    }
}

bool Daemon::loadConfig() {
    syslog(LOG_INFO, "Reading config file");
    std::map<Parser::Grammar::ConfigParams, std::string> config;
    Parser& parser = Parser::getInstance();
    if(!parser.parseConfig(_configFile)){
        throw std::runtime_error("Can't parse config");
    }
    config = parser.getParams();
    _delayTimeInterval = std::stoi(config[Parser::Grammar::ConfigParams::TIME_DELAY]);
    _dir1 = getAbsPath(config[Parser::Grammar::ConfigParams::DIRECTORY1]);
    _dir2 = getAbsPath(config[Parser::Grammar::ConfigParams::DIRECTORY2]);

    return true;
}

bool Daemon::setConfig(const std::string &configFile) {
    _configFile = getAbsPath(configFile);
    if (_configFile.empty()) {
        throw std::runtime_error("Wrong config file full path " +  _configFile);
    }
    syslog(LOG_INFO, "Config full path - %s", _configFile.c_str());
    return true;
}

std::string Daemon::getAbsPath(const std::string &path) {
    if (path.empty() || path[0] == '/') {
        return path;
    }
    std::string output = _homeDir + "/" + path;
    char *result = realpath(output.c_str(), nullptr);
    if (result == nullptr) {
        throw std::runtime_error("Couldn't find path:" + path);
    }
    std::string resultPath(result);
    delete result;
    return resultPath;
}

void Daemon::moveAllFiles(std::string const& src, std::string const& dst) {
    namespace fs = std::filesystem;
    for(fs::path p:fs::directory_iterator(src)){
        syslog(LOG_INFO, "Move file %s", p.c_str());
        fs::path destFile = dst/p.filename();
        if(fs::is_directory(p)){
            fs::create_directory(destFile);
            this->moveAllFiles(p.string(), destFile.string());
        } else{
            fs::rename(p,destFile);
        }
    }
}

void Daemon::removeAllFilesFromDir(std::string const& dir) {
    for (auto& path: std::filesystem::directory_iterator(dir)) {
        std::filesystem::remove_all(path);
    }
}

