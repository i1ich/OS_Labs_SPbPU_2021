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

#include "daemon.h"

daemon* daemon::daemonInstance;
bool daemon::isRunned;

bool daemon::runDaemon(const std::string& configFilePath)
{
    pid_t pid, sid;
    if (daemon && isRunned)
        return true;
    openlog("lab1", LOG_NOWAIT | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Initializing daemon");
    if (configFilePath.empty()) {
        syslog(LOG_ERR, "Config file path was not determined");
        return false;
    }
    pid = fork();
    if (pid == -1) {
        isRunned = false;
        syslog(LOG_ERR, "Start Daemon failed (%s)", strerror(errno));
        return false;
    }
    else if (pid)
        return true;
    isRunned = true;

    umask(0);
    syslog(LOG_INFO, "First fork was ended successfully");
    sid = setsid();
    if (sid < 0) {
        isRunned = false;
        syslog(LOG_ERR, "Could not generate session ID for child process");
        return false;
    }
    pid = fork();
    if (pid == -1) {
        isRunned = false;
        syslog(LOG_ERR, "Start Daemon failed (%s)", strerror(errno));
        return false;
    }
    if (pid)
        return true;
    syslog(LOG_INFO, "Second fork was ended successfully");
    if (!daemonInst) {
        daemonInst = new daemon();
    }
    if (!daemonInst) {
        isRunned = false;
        syslog(LOG_ERR, "Memory allocation error");
        return false;
    }
    if (!daemonInst->KillOldByPid()) {
        isRunned = false;
        return false;
    }
    syslog(LOG_INFO, "Second fork was ended successfully");
    if (!daemonInst->LoadConfig(configFilePath)) {
        isRunned = false;
        closelog();
        return false;
    }
    try {
        daemonInst->initializePath = fs::absolute(".").string() + '/';
    }
    catch (const fs::filesystem_error& error) {
        syslog(LOG_ERR, "During getting absolute path to running dir an error occurred %s", error.what());
        isRunned = false;
        kill(getpid(), SIGTERM);
    }
    if ((chdir("/")) < 0) {
        isRunned = false;
        syslog(LOG_ERR, "Could not change working directory to /");
        return false;
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, SignalHandler);
    signal(SIGTERM, SignalHandler);

    daemonInst->Execute();

    syslog(LOG_INFO, "Execution ended");
    closelog();
    return true;

}

bool daemon::LoadConfig(const std::string& configFilePath) 
{
    std::string line;
    this->configFileName = configFileName.empty() ? configFilePath : configFileName;
    std::string path = configFilePath.empty() ? initializePath + daemonInstance->configFileName : configFilePath;
    std::ifstream file(path);
    if (!file.is_open()) 
    {
        syslog(LOG_ERR, "Cant open config file");
        return false;
    }

    while (std::getline(file, line))
    {
        std::stringstream lineStream(line);
        std::vector<std::string> words;
        std::tm tm = {};
        lineStream >> std::get_time(&tm, "%d.%m.%Y %H:%M");
        event cur;
        //cur.startTime = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        cur.startTime = tm;

        char period;
        lineStream >> period;
        switch (period)
        {
        case 'm':
        case 'M':
            cur.period = REPEAT_MINUTE;
            break;
        case 'h':
        case 'H':
            cur.period = REPEAT_HOUR;
            break;
        case 'd':
        case 'D':
            cur.period = REPEAT_DAY;
            break;
        case 'w':
        case 'W':
            cur.period = REPEAT_WEEK;
            break;
        default:
            cur.period = REPEAT_ONCE;
            break;
        }

        while (!lineStream.eof())
        {
            std::string w;
            lineStream >> w;
            cur.text += w + ' ';
        }
        events.push_back(cur);
    }
    file.close();
    syslog(LOG_INFO, "Read %i events from config", daemonInstance->events.size());
    return true;
}

void daemon::SignalHandler(int signal) {
    switch (signal) {
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

bool daemon::SetPidFile(const std::string& filePath) {
    std::ofstream pidStream(filePath);
    if (!pidStream.is_open()) {
        syslog(LOG_ERR, "Pid file %s can not be opened", daemonInstance->pidFilePath.c_str());
        return false;
    }
    pid_t pid = getpid();
    pidStream << pid;
    pidStream.close();
    syslog(LOG_NOTICE, "New pid %d was wrote to .pid file", pid);
    return true;
}

bool daemon::KillOldByPid() {
    std::ifstream pidFile(pidFilePath);

    if (!pidFile.is_open()) {
        syslog(LOG_ERR, "Pid file %s can not be opened", pidFilePath.c_str());
        return false;
    }
    if (!pidFile.eof()) {
        pid_t oldPid;
        pidFile >> oldPid;
        auto proc = "/proc/" + std::to_string(oldPid);
        if (std::filesystem::exists(proc)) {
            kill(oldPid, SIGTERM);
            syslog(LOG_NOTICE, "Daemon with pid %d was killed", oldPid);
        }
    }
    pidFile.close();
    return SetPidFile(pidFilePath);
}

void daemon::Execute()
{
    syslog(LOG_INFO, "Daemon execution");
    while (1) {
        Idle();
        sleep(timeStamp);
    }
}

void daemon::Idle() 
{
    for (auto line : configLines) {
        cleanDir(initializePath + line[1]);
        copyFilesWithExt(initializePath + line[0], initializePath + line[1], line[2]);
    }
}

void Notify(std::string text);

bool daemon::CheckEvent(event ev)
{
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime_t = std::chrono::system_clock::to_time_t(now);
    std::tm *nowTime = localtime(&nowTime_t);

    if (std::chrono::system_clock::from_time_t(std::mktime(&ev.startTime)) > now)
        return false;

    switch (ev.period)
    {
    case REPEAT_MINUTE:
        return true;
    case REPEAT_HOUR:
        if (ev.startTime.tm_min == nowTime->tm_min)
            return true;
        return false;
    case REPEAT_DAY:
        if (ev.startTime.tm_min == nowTime->tm_min &&
            ev.startTime.tm_hour == nowTime->tm_hour)
            return true;
        return false;
    case REPEAT_WEEK:
        if (ev.startTime.tm_min == nowTime->tm_min &&
            ev.startTime.tm_hour == nowTime->tm_hour &&
            ev.startTime.tm_wday == nowTime->tm_wday)
            return true;
        return false;
    case REPEAT_ONCE:
        nowTime->tm_sec = ev.startTime.tm_sec; // for checking all without seconds
        if (ev.startTime == *nowTime)
            return true;
        return false;
    default:
        break;
    }

}

