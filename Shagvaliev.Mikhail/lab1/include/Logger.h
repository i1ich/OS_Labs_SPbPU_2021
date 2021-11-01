#pragma once

#include <map>
#include <iostream>
#include <memory>
#include <syslog.h>

class Logger {
public:
    static Logger& getInstance() {
        static Logger logger;
        return logger;
    }

    void init() {
        openlog("MY_DAEMON", LOG_NDELAY | LOG_PID, LOG_USER);
        isInitialized = true;
    }

    void close() {
        isInitialized = false;
        closelog();
    }

    void logError(const std::string& message) const { log(LOG_ERR, message); }
    void logWarn(const std::string& message) const { log(LOG_WARNING, message); };
    void logInfo(const std::string& message) const { log(LOG_INFO, message); };

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() = default;

    void log(int level, const std::string& message) const {
        if (not isInitialized)
            return;

        syslog(level, "%s", message.c_str());
    };

    bool isInitialized = false;
};
