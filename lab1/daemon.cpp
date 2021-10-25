
#include "daemon.h"
#include <csignal>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <fstream>
#include <map>
#include <dirent.h>
#include <cstring>
#include <iomanip>
#include "parser.h"


string daemon::configFile = "";
string daemon::pidFileName = "/var/run/IgnatievDeamon.pid";
string daemon::dirHome;
string daemon::dir1 = "dir1";
string daemon::dir2 = "dir2";
unsigned int daemon::waitingTime = 30;
unsigned int daemon::timeDifference = 600;

bool daemon::startDaemonization(const string& configFileParse)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("First fork failed");
        return false;
    }
    else if (pid != 0)
    {
        return true;
    }
    openlog("daemon", LOG_PID | LOG_NDELAY, LOG_USER);
    char buf[FILENAME_MAX];
    getcwd(buf, FILENAME_MAX);
    dirHome = buf;
    syslog(LOG_INFO, "Dir home - %s", buf);
    configFile = getFullPath(configFileParse);
    if (configFile.empty())
    {
        syslog(LOG_ERR, "Wrong config file full path - %s", configFileParse.c_str());
        return false;
    }

    signal(SIGTERM, signalHandler);
    signal(SIGHUP, signalHandler);

    try
    {
        loadConfig();
    }
    catch(exception& e)
    {
        syslog(LOG_ERR, "%s", e.what());
        clearDaemon();
    }

    if (setsid() == -1)
    {
        syslog(LOG_ERR, "Setsid return error: %d", errno);
        terminateDaemon();
    }
    pid = fork();
    if (pid == -1)
    {
        syslog(LOG_ERR, "Second fork failed");
        terminateDaemon();
    }

    if (pid == 0)
    {
        syslog(LOG_INFO, "Create daemon");
        umask(0);
        if (chdir("/") == -1)
        {
            syslog(LOG_ERR, "Chdir return error: %d", errno);
            terminateDaemon();
        }

        if (close(STDIN_FILENO) == -1 || close(STDOUT_FILENO) == -1 || close(STDERR_FILENO) == -1)
        {
            syslog(LOG_ERR, "Close return error: %d", errno);
            terminateDaemon();
        }

        startWork();
    }
    syslog(LOG_INFO, "Exit parent");
    clearDaemon();
    return true;
}

void daemon::clearDaemon()
{
    syslog(LOG_INFO, "Close log");
    closelog();
}

void daemon::terminateDaemon()
{
    clearDaemon();
    exit(SIGTERM);
}

string daemon::getFullPath(const string& path)
{
    if (path[0] == '/')
    {
        return path;
    }
    string fullPath = dirHome + "/" + path;
    char* realPath = realpath(fullPath.c_str(), nullptr);
    if (realPath == nullptr)
    {
        syslog(LOG_ERR, "Can't get full path - %s", fullPath.c_str());
        return string();
    }
    string resultPath(realPath);
    free(realPath);
    return resultPath;
}

void daemon::loadConfig()
{
    syslog(LOG_INFO, "Load config: %s", configFile.c_str());
    configData confData;
    try
    {
        confData = parser::parseConfigFile(configFile);
    }
    catch (exception & e)
    {
        throw e;
    }
    waitingTime = confData.waitingTime;
    timeDifference = confData.diffecrence;
    dir1 = confData.dir1;
    dir2 = confData.dir2;
    dir1 = getFullPath(dir1);
    dir2 = getFullPath(dir2);
    if (dir1.empty() || dir2.empty())
    {
        throw runtime_error("Wrong directory full path");
    }
    syslog(LOG_INFO, "waiting time = %d, difference = %d, dir1= %s, dir2= %s", waitingTime, timeDifference,dir1.c_str(), dir2.c_str());
}

bool daemon::checkPidFile()
{
    ifstream pidFile(pidFileName);
    if (!pidFile.is_open())
    {
        syslog(LOG_ERR, "Can't open pid file");
        return false;
    }
    pid_t oldPid;
    pidFile >> oldPid;
    pidFile.close();
    struct stat sb;
    string pathOldDaemon = "/proc/" + to_string(oldPid);
    if (stat(pathOldDaemon.c_str(), &sb) == 0)
    {
        kill(oldPid, SIGTERM);
    }
    return setPidFile();
}

bool daemon::setPidFile()
{
    ofstream pidFileOut(pidFileName, ofstream::out | ofstream::trunc);
    if (!pidFileOut.is_open())
    {
        syslog(LOG_ERR, "Can't open pid file out");
        return false;
    }
    pidFileOut << getpid();
    pidFileOut.close();
    return true;
}

void daemon::signalHandler(int signalNum)
{
    if (signalNum == SIGTERM)
    {
        terminateDaemon();
    }
    else if (signalNum == SIGHUP)
    {
        try
        {
            loadConfig();
        }
        catch(exception& e)
        {
            syslog(LOG_ERR, "%s", e.what());
            terminateDaemon();
        }
    }
    else
    {
        syslog(LOG_ERR, "Unknow signal - %d", signalNum);
    }
}

void daemon::startWork()
{
#ifdef PID_FILE
    pidFileName = PID_FILE;
#endif
    pidFileName = getFullPath(pidFileName);
    if (pidFileName.empty() || !checkPidFile())
    {
        terminateDaemon();
    }
    syslog(LOG_INFO, "Pid file - %s", pidFileName.c_str());
    syslog(LOG_INFO, "Start work");
    while (true)
    {
        makeAction();
        sleep(waitingTime);
    }
}

void daemon::dir1ToDir2()
{
    DIR * dir = opendir(dir1.c_str());
    if (!dir)
    {
        throw std::runtime_error("Can't open directory");
    }

    struct stat fileStat;
    struct dirent * ent;

    time_t createTime, curTime;
    time(&curTime);

    while ((ent = readdir(dir)) != nullptr)
    {
        if (ent->d_type == DT_DIR)
        {
            continue;
        }

        std::string filePathSrc, filePathDst;
        std::stringstream ss;
        ss << dir1 << "/" << ent->d_name;
        filePathSrc = ss.str();
        ss.str("");
        ss << dir2 << "/" << ent->d_name;
        filePathDst = ss.str();
        stat(filePathSrc.c_str(), &fileStat);
        createTime = fileStat.st_atime;

        if(difftime(curTime, createTime) < timeDifference)
        {
            syslog(LOG_INFO, "%s -> %s", filePathSrc.c_str(), filePathDst.c_str());
            moveFile(filePathSrc.c_str(), filePathDst.c_str());
        }
    }

    closedir(dir);
}

void daemon::dir2ToDir1()
{
    DIR * dir = opendir(dir2.c_str());
    if (!dir)
    {
        throw std::runtime_error("Can't open directory");
    }

    struct stat fileStat;
    struct dirent * ent;

    time_t createTime, curTime;
    time(&curTime);

    while ((ent = readdir(dir)) != nullptr)
    {
        if (ent->d_type == DT_DIR)
        {
            continue;
        }

        std::string filePathSrc, filePathDst;
        std::stringstream ss;
        ss << dir2 << "/" << ent->d_name;
        filePathSrc = ss.str();
        ss.str("");
        ss << dir1 << "/" << ent->d_name;
        filePathDst = ss.str();
        stat(filePathSrc.c_str(), &fileStat);
        createTime = fileStat.st_atime;

        syslog(LOG_INFO, "%s: %lf second since creating", filePathSrc.c_str(), difftime(curTime, createTime) );

        if(difftime(curTime, createTime) >= timeDifference)
        {
            syslog(LOG_INFO, "%s -> %s", filePathSrc.c_str(), filePathDst.c_str());
            moveFile(filePathSrc.c_str(), filePathDst.c_str());
        }
    }

    closedir(dir);
}

void daemon::makeAction()
{
    syslog(LOG_INFO, "Moving from dir1 to dir2...");
    dir1ToDir2();
    syslog(LOG_INFO, "Complete.");
    syslog(LOG_INFO, "Moving from dir2 to dir1...");
    dir2ToDir1();
    syslog(LOG_INFO, "Complete.");
}

void daemon::moveFile(const char* pathSrc, const char* pathDst)
{
    int src = open(pathSrc, O_RDONLY, 0);
    int dst = open(pathDst, O_WRONLY | O_CREAT, 0644);

    struct stat statSrc;
    fstat(src, &statSrc);

    sendfile(dst, src, nullptr, statSrc.st_size);

    close(src);
    close(dst);

    if (std::remove(pathSrc) != 0)
    {
        throw std::runtime_error("Can't move the file");
    }
}
