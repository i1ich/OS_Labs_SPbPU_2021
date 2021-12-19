#ifndef LAB2_LOGGER_H
#define LAB2_LOGGER_H

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

    void open(const char* name) {
        openlog(name, LOG_NDELAY | LOG_PID, LOG_USER);
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

        std::string levelString = "";
        switch (level) {
            case LOG_ERR:
                levelString = "ERROR";
                break;
            case LOG_WARNING:
                levelString = "WARNING";
                break;
            case LOG_INFO:
                levelString = "INFO";
                break;
        }
        std::cout << levelString << " " << message << std::endl;
    };

    bool isInitialized = false;
};


#endif //LAB2_LOGGER_H