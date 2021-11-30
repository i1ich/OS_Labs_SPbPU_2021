//
// Created by oleg on 22.10.2021.
//

#ifndef LAB1_DAEMON_H
#define LAB1_DAEMON_H


#include <string>
#include <map>
#include <sys/syslog.h>
#include <filesystem>
#include <unistd.h>
#include <sys/stat.h>
#include <csignal>
#include "Parser.h"

const char *const PID_FILE = "/var/run/lab1.pid";

class Daemon {
public:
    bool init(const std::string& conPath);
    void run();
    static Daemon* getInstance();
    static std::string getFullWorkingDirectory(std::string const& path);
    std::string getStartWorkingDirectory();

    ~Daemon() = default;

    Daemon(const Daemon&) = delete;
    Daemon& operator=(const Daemon&) = delete;

private:
    Daemon() = default;

    static Daemon instance;
    void work(const std::pair<std::string, int>& record);
    void recursiveDelete(const std::filesystem::path& path, int depth);
    void recursiveDeletePathFiles(const std::filesystem::path& path);
    void stopDaemon();
    static void signal_handler(int signal_id);
    bool setPidFile();
    bool checkPid(pid_t pid);
    static bool isDirectory(const std::string& path);

    Parser _parser;
    std::string startWorkingDirectory;
    std::string configPath;
    std::string pidPath;
    bool runDaemon{};
    bool readAgain{};
};


#endif //LAB1_DAEMON_H
