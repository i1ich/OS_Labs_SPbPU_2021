#pragma once

#include "definitions.h"
#include "Parser.h"

class Daemon {
public:
    static Daemon& getInstance();

    bool setUp(std::string const& configFile_);
    void run();
    void stop();

    Daemon(const Daemon&) = delete;
    Daemon& operator=(const Daemon&) = delete;

private:
    Daemon() = default;

    bool work();

    static void processSignal(int signal);

    bool validatePid(pid_t pid);
    bool processPid();
    bool setPid();

    std::string convertToFullPath(std::string const& path);

    bool loadConfig();

    std::string configFile;
    std::string pidFile;
    std::string homeDir;

    bool isRunning = false;

    int timeInterval;
    std::vector<DirectoryWithIgnFile> directoriesToCheck;
};
