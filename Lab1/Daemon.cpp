#include <syslog.h>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <bits/types/siginfo_t.h>
#include <csignal>
#include <wait.h>
#include "Daemon.h"

Daemon* Daemon::daemon;

bool Daemon::isRunned;

namespace fs = std::filesystem;
// Start Daemon
bool Daemon::runDaemon(const std::string& configFilePath)
{
    if (daemon && isRunned)
        return true;
    isRunned = true;
    openlog(    "lab1_16", LOG_NOWAIT | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Daemon initializing was started");
    if (configFilePath.empty()) {
        syslog(LOG_ERR, "Config file path wasn`t determined");
        return false;
    }
    pid_t pid, sid;
    pid = fork();
    if (pid == -1)
    {
        isRunned = false;
        syslog(LOG_ERR, "Start Daemon failed (%s)", strerror(errno));
        return false;
    }
    else if (pid)
        return true;

    umask(0);
    syslog(LOG_INFO, "Successfully1!!");
    sid = setsid();
    if (sid < 0)
    {
        isRunned = false;
        syslog(LOG_ERR, "Could not generate session ID for child process");
        return false;
    }
    pid = fork();
    if (pid == -1)
    {
        isRunned = false;
        syslog(LOG_ERR, "Start Daemon failed (%s)", strerror(errno));
        return false;
    }
    if (pid)
        return true;
    syslog(LOG_INFO, "Second fork was ended successfully");
    if (!daemon)
    {
        daemon = new Daemon();
    }
    if (!daemon)
    {
        isRunned = false;
        syslog(LOG_ERR, "Memory allocation error");
        return false;
    }
    if (!daemon->KillOldByPid()) {
        isRunned = false;
        return false;
    }
    syslog(LOG_INFO, "Third fork was ended successfully");
    if (!daemon->LoadConfig(configFilePath))
    {
        isRunned = false;
        closelog();
        return false;
    }
    try
    {
        daemon->initializePath = fs::absolute(".").string() + '/';
    }
    catch (const fs::filesystem_error& error)
    {
        syslog(LOG_ERR, "During getting absolute path to running dir an error occurred %s", error.what());
        isRunned = false;
        kill(getpid(), SIGTERM);
    }
    if((chdir("/")) < 0)
    {
        isRunned = false;
        syslog(LOG_ERR, "Could not change working directory to /");
        return false;
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, SignalHandler);
    signal(SIGTERM, SignalHandler);
    int status = daemon->Work();
    syslog(LOG_INFO, "Daemon work ended");
    closelog();
    return status;

}

bool Daemon::LoadConfig(const std::string& configFilePath)
{
    std::string line;
    this->configFileName = configFileName.empty() ? configFilePath : configFileName;
    std::string path = configFilePath.empty() ? initializePath + daemon->configFileName : configFilePath;
    std::ifstream file(path);
    if (!file.is_open())
    {
        syslog(LOG_ERR, "Config file could not be open");
        return false;
    }
    syslog(LOG_INFO, "Config file %s was opened", configFileName.c_str());
    std::vector<std::vector<std::string>> confLinesResult;
    while( std::getline(file, line) )
    {
        std::vector<std::string> args = split(line, configDelimeter);
        if (args.size() != 3)
        {
            syslog(LOG_ERR, "Config file incorrect: wrong arguments number per line");
            file.close();
            return false;
        }

        for (size_t i = 0; i < args.size(); i++)
        {
            args[i] = trim(args[i]);
            if (i < 2)
                if (!fs::is_directory(initializePath + args[i]))
                {
                    syslog(LOG_ERR, "Config file incorrect: two first argument in each line must be"
                                    " existing directories");
                    file.close();
                    return false;
                }
        }
        confLinesResult.push_back(args);
    }
    file.close();
    configLines.clear();
    configLines.insert(configLines.end(), confLinesResult.begin(), confLinesResult.end());
    syslog(LOG_INFO, "Config file was successfully read");
    return true;
}

void Daemon::SignalHandler(int signal)
{
    switch (signal)
    {
        case SIGHUP:
            if (!daemon->LoadConfig())
                syslog(LOG_ERR, "Config file was not updated");
            else
                syslog(LOG_INFO, "Config file was updated");
            break;
        case SIGTERM:
            isRunned = false;
            syslog(LOG_INFO, "Daemon was stopped");
            closelog();
            exit(0);
        default:
            syslog(LOG_INFO, "Signal %i was not handled", signal);
    }
}

bool Daemon::SetPidFile(const std::string& filePath)
{
    std::ofstream pidStream(filePath);
    if (!pidStream.is_open())
    {
        syslog(LOG_ERR, "Pid file %s can not be opened", daemon->pidFilePath.c_str());
        return false;
    }
    pid_t pid = getpid();
    pidStream << pid;
    pidStream.close();
    syslog(LOG_NOTICE, "New pid %d was wrote to .pid file", pid);
    return true;
}

bool Daemon::KillOldByPid()
{
    std::ifstream pidFile(pidFilePath);

    if (!pidFile.is_open())
    {
        syslog(LOG_ERR, "Pid file %s can not be opened", pidFilePath.c_str());
        return false;
    }
    if (!pidFile.eof())
    {
        pid_t oldPid;
        pidFile >> oldPid;
        auto proc = "/proc/" + std::to_string(oldPid);
        if (fs::exists(proc))
        {
            kill(oldPid, SIGTERM);
            syslog(LOG_NOTICE, "Daemon with pid %d was killed", oldPid);
        }
    }
    pidFile.close();
    return SetPidFile(pidFilePath);
}

int Daemon::Work()
{
    syslog(LOG_NOTICE, "Daemon work started");

    while(true)
    {
        do_heartbeat();
        sleep(SleepTimeInterval);
    }
}

void Daemon::do_heartbeat()
{
    syslog(LOG_INFO, "Daemon heart beat with %ld lines", configLines.size());
    for (auto line : configLines)
    {
        cleanDir(initializePath + line[1]);
        copyFilesWithExt(initializePath + line[0], initializePath + line[1], line[2]);
    }
}

void Daemon::copyFilesWithExt(const std::string& sourceDirPath, const std::string& targetDirPath, const std::string& fileExtension)
{
    try
    {
        for (auto const &file: fs::directory_iterator(sourceDirPath))
        {
            if (file.is_directory())
                continue;
            if (fileExtension == fs::path(file).extension())
            {
                auto targetFilePath = targetDirPath + "/" + fs::path(file).filename().c_str();
                fs::copy(file, targetFilePath);
            }
        }
    }
    catch (const fs::filesystem_error& error) {
        syslog(LOG_ERR, "During copying files from %s to %s an error occurred %s",
               sourceDirPath.c_str(), targetDirPath.c_str(), error.what());
        isRunned = false;
        kill(getpid(), SIGTERM);
    }
}

void Daemon::cleanDir(const std::string& dirPath)
{
    try {
        for (auto const &file: fs::directory_iterator(dirPath)) {
            if (file.is_directory())
                fs::remove_all(file);
            else
                fs::remove(file);
        }
    }
    catch (const fs::filesystem_error& error)
    {
        syslog(LOG_ERR, "During cleaning dir %s an error occurred %s", dirPath.c_str(), error.what());
        isRunned = false;
        kill(getpid(), SIGTERM);
    }
}
