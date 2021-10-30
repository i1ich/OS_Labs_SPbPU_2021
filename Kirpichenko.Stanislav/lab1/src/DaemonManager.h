#pragma once
#include <string>
#include <fstream>

#include "Singleton.h"

class DaemonManager: public Singleton<DaemonManager> {
private:
    static const std::string_view _pidFilename;
    std::string _cfgPath;

    void writePID(std::ofstream& pidFile);
    int readPID(std::ifstream& pidFile);
    static void signalHandler(int signal);
    void findConfig();
public:
    void startNewSession(const char* cfgPath);
    void setConfigPath(const char* path);
    const char* getConfigPath();
};
