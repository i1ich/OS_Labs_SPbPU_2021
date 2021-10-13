#include "daemon.h"

std::string Daemon::configFile_;
std::string Daemon::pidFile_ = "/var/run/lab1.pid";
std::string Daemon::homeDir_;

int Daemon::sleepTimeInterval_;
std::string Daemon::firstDir_;
std::string Daemon::secondDir_;
bool Daemon::workStatus_;

std::string Daemon::IMGFolder = "IMG";
std::string Daemon::OTHERSFolder = "OTHERS";


bool Daemon::init(std::string const& configFile) {
    openlog((new std::string("MY_DAEMON"))->c_str(), LOG_NDELAY | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "INFO: Initializing daemon");

    workStatus_ = true;

    char buff[FILENAME_MAX];

    if(!getcwd(buff, FILENAME_MAX)) {
        syslog(LOG_ERR, "ERROR: Failed in getcwd: %d", errno);
        stopDaemon();
        return false;
    }

    homeDir_ = buff;

    configFile_ = getFullPath(configFile);

    if(!loadConfig()) {
        syslog(LOG_ERR, "ERROR: Failed in setConfig");
        stopDaemon();
        return false;
    }

    pid_t pid = fork();

    if (!checkPid(pid)) {
        return false;
    }

    syslog(LOG_INFO, "INFO: First fork success");

    if (setsid() == -1) {
        syslog(LOG_ERR, "ERROR: Failed in setsid: %d", errno);
        stopDaemon();
        return false;
    }

    pid = fork();

    if (!checkPid(pid)) {
        return false;
    }

    syslog(LOG_INFO, "INFO: Second fork success");

    umask(0);

    if ((chdir("/")) == -1) {
        syslog(LOG_ERR, "ERROR: Failed in chdir: %d", errno);
        stopDaemon();
        return false;
    }

    int fd;
    /* Close all open file descriptors */
    for (fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--) {
        close(fd);
    }

    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);

    if(!handlePid()) {
        syslog(LOG_ERR, "ERROR: Failed in handlePidFile");
        stopDaemon();
        return false;
    }

    if(!loadConfig()){
        syslog(LOG_ERR, "ERROR: Failed in setConfig");
        stopDaemon();
        return false;
    }

    syslog(LOG_INFO, "INFO: Daemon is successfully initialized");

    return true;
}


void Daemon::run() {
    while (workStatus_ && work()) {
        sleep(sleepTimeInterval_);
    }
}


bool Daemon::work() {
    removeAllFilesFromDir(secondDir_);
    moveFiles(firstDir_, secondDir_);
    return true;
}


void Daemon::stopDaemon() {
    workStatus_ = false;
    syslog(LOG_INFO, "INFO: Stop Daemon");
    closelog();
}


bool Daemon::loadConfig() {
    syslog(LOG_INFO, "INFO: Loading config");

    if (!Parser::parseConfig(configFile_)) {
        syslog(LOG_ERR, "ERROR: Parsing config");
        return false;
    }

    std::map<Parser::configParameters, std::string> parameter = Parser::getParameters();
    sleepTimeInterval_ = std::stoi(parameter[Parser::TIME]);
    firstDir_ = getFullPath(parameter.at(Parser::DIR1));
    secondDir_ = getFullPath(parameter.at(Parser::DIR2));

    return true;
}


std::string Daemon::getFullPath(std::string const& path) {
    if (path.empty() || path[0] == '/') {
        return path;
    }
    std::string fullPath = homeDir_ + "/" + path;
    char* realPath = realpath(fullPath.c_str(), nullptr);
    if (realPath == nullptr) {
        syslog(LOG_ERR, "ERROR: While getting full path");
        return "";
    }
    std::string resultPath(realPath);
    free(realPath);
    return resultPath;
}


bool Daemon::handlePid() {
    syslog(LOG_INFO, "INFO: Handle PID file");
    pidFile_ = getFullPath(pidFile_);
    std::ifstream pidFile(pidFile_);

    if (!pidFile.is_open()) {
        syslog(LOG_INFO, "INFO: Cannot open old pid file");

    } else if (!pidFile.eof()) {
        pid_t oldPid;
        pidFile >> oldPid;
        pidFile.close();

        struct stat sb;
        std::string oldPath = "/proc/" + std::to_string(oldPid);

        if (stat(oldPath.c_str(), &sb) == 0)
            kill(oldPid, SIGTERM);
    }

    return setPid();
}


bool Daemon::setPid() {
    std::ofstream pidStream(pidFile_);
    if (!pidStream.is_open()) {
        syslog(LOG_ERR, "ERROR: Cannot open pid file");
        return false;
    }
    pidStream << getpid();
    pidStream.close();
    return true;
}


void Daemon::signalHandler(int signal) {
    switch (signal) {
        case SIGHUP:
            syslog(LOG_INFO, "INFO: Updating config file");
            if (!loadConfig()) {
                stopDaemon();
            }
            break;
        case SIGTERM:
            stopDaemon();
            break;
        default:
            syslog(LOG_INFO, "INFO: Signal %i is not handled", signal);
    }
}


bool Daemon::checkPid(pid_t pid) {
    if (pid == -1) {
        syslog(LOG_ERR, "ERROR: Creating fork");
        stopDaemon();
        return false;
    } else if (pid != 0) {
        syslog(LOG_INFO, "INFO: Now in the parent process");
        return false;
    }
    return true;
}


void Daemon::removeAllFilesFromDir(std::string const& dir) {
    DIR *theFolder = opendir(dir.c_str());
    struct dirent *next_file;
    char filepath[256];

    while ( (next_file = readdir(theFolder)) != nullptr ) {
        // build the path for each file in the folder
        sprintf(filepath, "%s/%s", dir.c_str(), next_file->d_name);
        remove(filepath);
    }
    closedir(theFolder);
}


void Daemon::moveFilesRecursively(std::string const& fromDir, std::string const& toDir,
                                  std::string const& pathToIMG, std::string const& pathToOTHERS) {
    DIR* src = opendir(fromDir.c_str());
    if (src == nullptr) {
        syslog(LOG_ERR, "ERROR: fail to open dir : %s", fromDir.c_str());
    }

    struct dirent* curr = readdir(src);

    while (curr != nullptr) {
        if (strcmp(curr->d_name, ".") != 0 && strcmp(curr->d_name, "..") != 0) {
            std::string fullPathFrom = fromDir + "/" + curr->d_name;
            if (exists(fullPathFrom) && isDir(fullPathFrom)) {
                moveFilesRecursively(fullPathFrom, toDir, pathToIMG, pathToOTHERS);
            }

            if (exists(fullPathFrom) && isFile(fullPathFrom)) {
                std::ifstream src_stream(fullPathFrom, std::ios::binary);
                if (!(src_stream.is_open())) {
                    syslog(LOG_ERR, "ERROR: cannot open src file stream");
                    return;
                }

                std::string fullPathTo = toDir + "/" + getFolderName(fullPathFrom) + "/" + curr->d_name;
                std::ofstream dest_stream(fullPathTo, std::ios::binary);
                if (!(dest_stream.is_open())) {
                    syslog(LOG_ERR, "ERROR: cannot open dst file stream");
                    src_stream.close();
                    return;
                } else {
                    dest_stream << src_stream.rdbuf();
                }
                src_stream.close();
                dest_stream.close();
            }
        }
        curr = readdir(src);
    }
    closedir(src);
}


void Daemon::moveFiles(std::string const& fromDir, std::string const& toDir) {
    std::string pathToIMG = toDir + "/" + IMGFolder;
    std::string pathToOTHERS = toDir + "/" + OTHERSFolder;

    mkdir(pathToIMG.c_str(), ACCESSPERMS);
    mkdir(pathToOTHERS.c_str(), ACCESSPERMS);

    syslog(LOG_INFO, "INFO: copy files from %s to %s", fromDir.c_str(), toDir.c_str());

    moveFilesRecursively(fromDir, toDir, pathToIMG, pathToOTHERS);
}


bool Daemon::exists(std::string const& path) {
    struct stat s{};
    return stat(path.c_str(),&s) == 0;
}


bool Daemon::isFile(std::string const& path) {
    struct stat s{};
    if( stat(path.c_str(),&s) == 0 ) {
        if (s.st_mode & S_IFREG) {
            return true;
        }
    }
    return false;
}


bool Daemon::isDir(std::string const& path) {
    struct stat s{};
    if( stat(path.c_str(),&s) == 0 ) {
        if (s.st_mode & S_IFDIR) {
            return true;
        }
    }
    return false;
}


const std::string& Daemon::getFolderName(std::string const& path) {
    if(path.substr(path.find_last_of('.') + 1) == "png") {
        return IMGFolder;
    } else {
        return OTHERSFolder;
    }
}