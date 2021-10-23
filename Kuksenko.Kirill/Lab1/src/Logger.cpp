#include "Logger.h"

#include <iostream>

Logger* Logger::inst = nullptr;


Logger* Logger::instance() {
    if (!inst) {
        inst = new Logger();
    }

    return inst;
}

void Logger::release() {
    if (inst) {
        delete inst;
        inst = nullptr;
    }
}

Logger::Logger() {
    openlog(NULL, LOG_PID, 0);
}

Logger::~Logger() {
    closelog();
}

void Logger::log(LEVEL level, std::string const& message) const {
    switch (level) {
    case LEVEL::INFO:
        syslog(LOG_INFO, ": %s", message.c_str());
        break;
    
    case LEVEL::WARNING:
        syslog(LOG_WARNING, ": %s", message.c_str());
        break;

    case LEVEL::ERROR:
        syslog(LOG_ERR, ": %s", message.c_str());
        break;

    default:
        break;
    }
}
