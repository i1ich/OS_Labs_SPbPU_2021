//
// Created by pikabol88 on 10/20/21.
//

#ifndef DAEMON_LAB_DAEMON_H
#define DAEMON_LAB_DAEMON_H

#include "Parser.h"

#include <csignal>
#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>
#include <map>
#include <dirent.h>
#include <filesystem>
#include <iostream>
#include <string>

class Daemon {
public:
    void init(const std::string &config);
    void run();
    void terminate();

    static Daemon& getInstance();

private:
    static Daemon _instance;

    std::string _configFile;
    std::string  _pidFileName;
    std::string _homeDir;

    std::string _dir1;
    std::string _dir2;
    unsigned int _delayTimeInterval;

    bool _isRunning = false;

    Daemon(){}
    Daemon(Daemon const&) = delete;
    Daemon & operator=(Daemon const&) = delete;

    void setConfig(const std::string &configFile);
    void loadConfig();
    static void signalHandler(int signalNum);

    bool initPid();
    bool initTread();
    void initSignals();

    std::string getAbsPath(const std::string &path);

    void checkPid();
    void savePid();

    void moveAllFiles(const std::string& src, const std::string& dst);
    void removeAllFilesFromDir(const std::string &dir);
};

#endif //DAEMON_LAB_DAEMON_H
