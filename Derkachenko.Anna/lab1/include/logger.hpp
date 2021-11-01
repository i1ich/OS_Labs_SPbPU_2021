#pragma once
#include <iostream>
#include <syslog.h>

class Logger
{
private:
    bool isSyslogOpen = false;
    const std::string syslogProcName = "myDaemon";

public:
    void openSyslog()
    {
        if (!isSyslogOpen)
            openlog(syslogProcName.c_str(), LOG_PID, LOG_DAEMON);
        isSyslogOpen = true;
    }

    void closeSyslog()
    {
        if (isSyslogOpen)
            closelog();
        isSyslogOpen = false;
    }

    void log(const std::string &msg)
    {
        if (isSyslogOpen)
            syslog(LOG_NOTICE, msg.c_str());
        else
            std::cout << msg << std::endl;
    }
};