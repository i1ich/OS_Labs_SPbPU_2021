#ifndef DAEMON_H
#define DAEMON_H


#include <string>
#include <vector>

std::string trim(const std::string &s);
std::vector<std::string> split(const std::string &s, const std::string & delimiter);

class Daemon {
public:
    static bool runDaemon(const std::string& configFilePath = "");
    Daemon(const Daemon& daemon) = delete;
    Daemon& operator=(const Daemon&) = delete;
private:
    static Daemon* daemon;
    static bool isRunned;
    std::string pidFilePath = "/var/run/lab1_var16.pid";
    std::string configFileName;
    std::string initializePath;
    int sleepTimeInterval = 10; // 10 seconds

    std::string configDelimeter = ";";
    std::vector<std::vector<std::string>> configLines;

    void do_heartbeat();
    static void SignalHandler(int signal);
    int Work();
    static void cleanDir(const std::string& dirPath);
    static void copyFilesWithExt(const std::string& sourceDirPath, const std::string& targetDirPath,
                          const std::string& fileExtension);

    bool LoadConfig(const std::string& configFilePath = "");
    static bool SetPidFile(const std::string& filePath);
    bool KillOldByPid();

    Daemon() = default;
};


#endif //DAEMON_H
