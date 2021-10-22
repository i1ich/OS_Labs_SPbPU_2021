#pragma once
#include "config.hpp"

class Daemon
{
private:
    static std::string configPath;
    static Config config;
    std::string pidFilePath;
    std::string logFile = "hist.log";
    std::string procDir = "/proc";
    std::string syslogProcName = "myDaemon";

protected:
    Daemon(const std::string& path);

    static void signalHandler(int signal);

    void closeRunning();
    void daemonize();

public:
    static Daemon &create(const std::string &path);

    void start();

    Daemon() = delete;
    Daemon(const Daemon &) = delete;
    Daemon(Daemon &&) = delete;
};
