#include "daemon.hpp"
#include <chrono>
#include <csignal>
#include <ctime>
#include <filesystem>
#include <sys/stat.h>
#include <syslog.h>
#include <unistd.h>

namespace fs = std::filesystem;

std::string Daemon::configPath;
Config Daemon::config;

Daemon& Daemon::create(const std::string& path)
{
    // local static variable ensures memory is freed when program terminates
    // initialized on first use and never after
    static Daemon instance(path);
    return instance;
}

Daemon::Daemon(const std::string& path)
{
    configPath = fs::absolute(path);
    pidFilePath = fs::absolute("myDaemon.txt");
    config = Config(configPath);
}

void Daemon::start()
{
    closeRunning();
    daemonize();
    syslog(LOG_NOTICE, "Daemon started");

    while (true)
    {
        std::ofstream f(config.getOutputDir() + "/" + logFile, std::ios_base::app);
        std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        f << std::ctime(&currentTime);
        for (const auto &entry : fs::directory_iterator(config.getInputDir()))
            f << entry.path() << std::endl;
        f << std::endl;
        sleep(config.getTime());
    }
}

void Daemon::closeRunning()
{
    int pid;
    std::ifstream f(pidFilePath);
    f >> pid;
    if (fs::exists(procDir + "/" + std::to_string(pid)))
        kill(pid, SIGTERM);
}

void Daemon::signalHandler(int signal)
{
    if (signal == SIGHUP)
        config.read(configPath);
    if (signal == SIGTERM)
    {
        syslog(LOG_NOTICE, "Deamon terminated");
        closelog();
        exit(EXIT_SUCCESS);
    }
}

void Daemon::daemonize()
{
    pid_t pid = fork();
    if (pid != 0)
        exit(EXIT_FAILURE);
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    std::signal(SIGHUP, signalHandler);
    std::signal(SIGTERM, signalHandler);

    pid = fork();
    if (pid != 0)
        exit(EXIT_FAILURE);
    umask(0);
    if (chdir("/") != 0)
        exit(EXIT_FAILURE);

    for (long x = sysconf(_SC_OPEN_MAX); x >= 0; --x)
        close(x);
    openlog(syslogProcName.c_str(), LOG_PID, LOG_DAEMON);

    std::ofstream f(pidFilePath, std::ios_base::trunc);
    f << getpid();
}
