
#ifndef __DAEMON_H_
#define __DAEMON_H_

#include <string>
#include <vector>

std::string trim(const std::string &s);
std::vector<std::string> split(const std::string &s, const std::string & delimiter);
// Daemon class
class Daemon {
public:
    static bool runDaemon(const std::string& configFilePath = "");
    // Singleton
    Daemon(const Daemon& daemon) = delete;
    Daemon& operator=(const Daemon&) = delete;
private:
    // Singleton
    static Daemon* daemon;
    static bool isRunned;
    std::string pidFilePath = "/var/run/lab1_var16.pid";
    std::string configFileName;
    std::string initializePath;
    int SleepTimeInterval = 10; // 10 seconds

    std::string configDelimeter = ";";
    std::vector<std::vector<std::string>> configLines;

    void do_heartbeat();
    static void SignalHandler(int signal);
    int Work();
    static void cleanDir(const std::string& dirPath);
    static void copyFilesWithExt(const std::string& sourceDirPath, const std::string& targetDirPath, const std::string& fileExtension);

    bool LoadConfig(const std::string& configFilePath = "");
    static bool SetPidFile(const std::string& filePath);
    bool KillOldByPid();

    Daemon() = default;
};


#endif // __DAEMON_H_
