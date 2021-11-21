#include <syslog.h>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <experimental/filesystem>
#include <bits/types/siginfo_t.h>
#include <csignal>
#include <wait.h>
#include <iomanip>

#include "daemon.h"

bool daemon::isRunned;
class daemon * daemon::daemonInstance = nullptr;

bool daemon::RunDaemon(const std::string& configFilePath)
{
    pid_t pid, sid;

    if (daemonInstance && isRunned)
        return true;

    openlog("lab1", LOG_NOWAIT | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Initializing daemon");

    if (configFilePath.empty())
    {
        syslog(LOG_ERR, "Config file path is empty");
        return false;
    }

    pid = fork();

    if (pid == -1)
    {
        isRunned = false;
        syslog(LOG_ERR, "Error while starting daemon: %s", strerror(errno));
        return false;
    }
    else if (pid)
        return true;

    isRunned = true;

    umask(0);
    sid = setsid();
    if (sid < 0)
    {
        isRunned = false;
        syslog(LOG_ERR, "Cant generate sID");
        return false;
    }

    pid = fork();

    if (pid == -1)
    {
        isRunned = false;
        syslog(LOG_ERR, "Error while starting daemon: %s", strerror(errno));
        return false;
    }

    if (pid)
        return true;

    if (!daemonInstance)
        daemonInstance = new daemon();

    if (!daemonInstance)
    {
        isRunned = false;
        syslog(LOG_ERR, "Memory allocation error");
        return false;
    }
    if (!daemonInstance->KillOldByPid())
    {
        isRunned = false;
        return false;
    }
    if (!daemonInstance->LoadConfig(configFilePath))
    {
        isRunned = false;
        closelog();
        return false;
    }
    try
    {
        daemonInstance->initializePath = std::experimental::filesystem::absolute(".").string() + '/';
    }
    catch (const std::experimental::filesystem::filesystem_error& error)
    {
        syslog(LOG_ERR, "Error while build absolute path: %s", error.what());
        isRunned = false;
        kill(getpid(), SIGTERM);
    }
    if ((chdir("/")) < 0)
    {
        isRunned = false;
        syslog(LOG_ERR, "Cant change work dir to /");
        return false;
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, SignalHandler);
    signal(SIGTERM, SignalHandler);

    daemonInstance->Execute();

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
    return true;
}

void daemon::SignalHandler(int signal)
{
    switch (signal)
    {
    case SIGHUP:
        daemonInstance->events.clear();
        if (!daemonInstance->LoadConfig(""))
            syslog(LOG_ERR, "Cant load config");
        break;
    case SIGTERM:
        isRunned = false;
        delete daemonInstance;
        syslog(LOG_INFO, "Daemon was stopped");
        closelog();
        exit(0);
    }
}

bool daemon::SetPidFile(const std::string& filePath)
{
    std::ofstream pidFile(filePath);
    if (!pidFile.is_open())
    {
        syslog(LOG_ERR, "Cant open pid file");
        return false;
    }
    pid_t pid = getpid();
    pidFile << pid;
    pidFile.close();
    return true;
}

bool daemon::KillOldByPid()
{
    std::ifstream pidFile(pidFilePath);

    if (!pidFile.is_open())
    {
        syslog(LOG_ERR, "Cant open pid file");
        return false;
    }
    if (!pidFile.eof())
    {
        pid_t oldPid;
        pidFile >> oldPid;
        std::string path = "/proc/" + std::to_string(oldPid);
        if (std::experimental::filesystem::exists(path))
        {
            kill(oldPid, SIGTERM);
            syslog(LOG_NOTICE, "Kill old pid %i", oldPid);
        }
    }
    pidFile.close();
    return SetPidFile(pidFilePath);
}

void daemon::Execute()
{
    syslog(LOG_INFO, "Daemon execution");
    while (1) 
    {
        Idle();
        sleep(timeStamp);
    }
}

void daemon::Idle() 
{
    for (auto ev : events)
        if (CheckEvent(ev))
            Notify(ev.text);
}

void daemon::Notify(std::string text)
{
    std::string str_xterm = "xterm -home -e command" + text;
    std::string str_kde = "konsole --noclose -e echo " + text;
    std::string str_gnome = "gnome-terminal -e \"bash -c 'echo " + text + " && sleep 3'\"" ;
    
    // one of them will probably work
    // I checked KDE one
    system(str_xterm.c_str());
    system(str_kde.c_str());
    system(str_gnome.c_str());
}

bool daemon::CheckEvent(event ev)
{
    auto now = std::chrono::system_clock::now();
    std::time_t fuckedNow = std::chrono::system_clock::to_time_t(now);
    std::tm *nowTime = localtime(&fuckedNow);

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
        if (ev.startTime.tm_min == nowTime->tm_min &&
            ev.startTime.tm_hour == nowTime->tm_hour &&
            ev.startTime.tm_yday == nowTime->tm_yday &&
            ev.startTime.tm_year == nowTime->tm_year)
            return true;
        return false;
    }
    return false;
}

