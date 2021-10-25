#include "../include/definitions.h"
#include "../include/Daemon.h"

bool exists(const std::string&path);
bool isFile(const std::string&path);
bool isDir(const std::string&path);

Daemon& Daemon::getInstance() {
    static Daemon instance;
    return instance;
}

bool Daemon::setUp(const std::string& configFile_) {
    auto& logger = Logger::getInstance();
    pidFile = "/var/run/lab1.pid";

    logger.logInfo("Initializing daemon");
    isRunning = true;

    char buff[FILENAME_MAX];

    if(!getcwd(buff, FILENAME_MAX)) {
        logger.logError("Failed in getcwd: " + std::to_string(errno));
        stop();
        return false;
    }

    homeDir = buff;
    configFile = convertToFullPath(configFile_);

//
//    pid_t pid = fork();
//
//    if (!validatePid(pid)) {
//        return false;
//    }
//
//    logger.logInfo("First fork success");
//
//    if (setsid() == -1) {
//        logger.logError("Failed in setsid: " + std::to_string(errno));
//        stop();
//        return false;
//    }
//
//    pid = fork();
//
//    if (!validatePid(pid)) {
//        return false;
//    }
//
//    logger.logInfo("Second fork success");
//
//    umask(0);
//
//    if ((chdir("/")) == -1) {
//        logger.logError("Failed in chdir: " + std::to_string(errno));
//        stop();
//        return false;
//    }
//
//    int fd;
//    for (fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--) {
//        close(fd);
//    }
//
//    for (int i = getdtablesize(); i >= 0; --i) {
//        close(i);
//    }
//
//    signal(SIGHUP, processSignal);
//    signal(SIGTERM, processSignal);
//
//    if(!processPid()) {
//        logger.logError("Failed in handlePidFil");
//        stop();
//        return false;
//    }

    if(!loadConfig()) {
        logger.logError("Failed loading config");
        stop();
        return false;
    }

    logger.logInfo("Daemon successfully initialized");

    return true;
}

void Daemon::run() {
    while (isRunning and work()) {
        sleep(timeInterval);
    }
}

bool Daemon::work() {
    auto& logger = Logger::getInstance();
    for (auto& it : directoriesToCheck) {
        auto directory = convertToFullPath(it.directory);
        if (not isDir(directory)) {
            logger.logError(directory + " is not a directory");
            return false;
        }

        auto fullIgnFilepath = directory + "/"  + it.ignFilename;
        if (not isFile(fullIgnFilepath)) {
            logger.logInfo("Cleaning directory " + directory);
            for (auto& path: std::filesystem::directory_iterator(directory)) {
                std::filesystem::remove_all(path);
            }
        }
        else {
            logger.logInfo("Not cleaning directory "+ directory + ", because ignfile " + it.ignFilename + " is present");
        }
    }
    return true;
}

void Daemon::stop() {
    isRunning = false;
    Logger::getInstance().logInfo("Stopped Daemon");
}

bool Daemon::loadConfig() {
    auto& logger = Logger::getInstance();
    logger.logInfo("Loading config");

    auto& parser = Parser::getInstance();
    if (!parser.parseConfig(configFile)) {
        logger.logError("Failed parsing config");
        return false;
    }

    timeInterval = parser.getTimeInterval();
    directoriesToCheck = parser.getDirectories();
    return true;
}

std::string Daemon::convertToFullPath(const std::string& path) {
    if (path.empty() or path[0] == '/') {
        return path;
    }
    auto fullPath = homeDir + "/" + path;
    char* realPath = realpath(fullPath.c_str(), nullptr);
    if (realPath == nullptr) {
        Logger::getInstance().logError("Failed getting full path");
        return "";
    }
    std::string resultPath(realPath);
    free(realPath);
    return resultPath;
}

bool Daemon::processPid() {
    auto& logger = Logger::getInstance();
    logger.logInfo("Processing PID file");
    pidFile = convertToFullPath(pidFile);
    std::ifstream pidFileStream(pidFile);

    if (!pidFileStream.is_open()) {
        logger.logInfo("Can not open old pid file");

    } else if (!pidFileStream.eof()) {
        pid_t oldPid;
        pidFileStream >> oldPid;
        pidFileStream.close();

        std::string oldPath = "/proc/" + std::to_string(oldPid);
        if (exists("/proc/" + std::to_string(oldPid))) {
            kill(oldPid, SIGTERM);
        }
    }

    return setPid();
}

bool Daemon::setPid() {
    std::ofstream pidStream(pidFile);
    if (!pidStream.is_open()) {
        Logger::getInstance().logError("Can not open pid file");
        return false;
    }
    pidStream << getpid();
    pidStream.close();
    return true;
}

void Daemon::processSignal(int signal) {
    auto& logger = Logger::getInstance();
    auto& daemon = getInstance();
    switch (signal) {
        case SIGHUP:
            logger.logInfo("Updating config file");
            if (daemon.loadConfig()) {
                daemon.stop();
            }
            break;
        case SIGTERM:
            daemon.stop();
            break;
        default:
            logger.logInfo("Signal " + std::to_string(signal)+ " is not processed");
    }
}


bool Daemon::validatePid(pid_t pid) {
    auto& logger = Logger::getInstance();
    if (pid == -1) {
        logger.logError("Failed creating fork");
        stop();
        return false;
    } else if (pid != 0) {
        logger.logInfo("Now in the parent process");
        return false;
    }
    return true;
}

bool exists(const std::string& path) {
    struct stat s{};
    return stat(path.c_str(),&s) == 0;
}

bool isFile(const std::string&path) {
    struct stat s{};
    if (stat(path.c_str(), &s) == 0) {
        if (s.st_mode & S_IFREG) {
            return true;
        }
    }
    return false;
}

bool isDir(const std::string& path) {
    struct stat s{};
    if (stat(path.c_str(), &s) == 0) {
        if (s.st_mode & S_IFDIR) {
            return true;
        }
    }
    return false;
}