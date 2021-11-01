#pragma once
#include "config.hpp"
#include "logger.hpp"

class Daemon
{
private:
    static Logger logger;
    static std::string configPath;
    static Config config;
    std::string pidFilePath;
    const std::string logFile = "hist.log";
    const std::string procDir = "/proc";

    Daemon(const std::string &path);

protected:
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
