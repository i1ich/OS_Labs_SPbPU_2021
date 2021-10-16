#include "Logger.h"

Logger::Logger(std::string const& name) {
    openlog(name.c_str(), LOG_PID, 0);
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

    case LEVEL::ERROR:
        syslog(LOG_ERR, ": %s", message.c_str());

    default:
        break;
    }
}
