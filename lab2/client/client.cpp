#include <string>
#include <sys/syslog.h>
#include <iostream>

#include "oracle.h"

int main(int argc, char*argv[])
{
    openlog("lab2_client", LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_USER);

    if (argc != 2)
    {
        syslog(LOG_ERR, "expected host pid as the only command argument");
        closelog();
        return 1;
    }

    int pid;
    try
    {
        pid = std::stoi(argv[1]);
    }
    catch (std::exception &e)
    {
        syslog(LOG_ERR, "An error while pid reading %s", e.what());
        closelog();
        return 1;
    }

    try
    {
        oracle::Run(pid);
    }
    catch (std::exception &e)
    {
        syslog(LOG_ERR, "An error %s", e.what());
    }
    closelog();
    return 0;
}

