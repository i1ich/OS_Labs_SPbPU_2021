#pragma once

#include <string>
#include <syslog.h>

class Logger {
public:
    enum LEVEL {
        INFO = 0,
        WARNING,
        ERROR,
    };

private:
    static Logger* inst;

public:
    static Logger* instance();
    static void release();
    
    void log(LEVEL level, std::string const& message) const;

private:
    Logger();
    ~Logger();

    Logger(Logger const&) = delete;
    Logger& operator=(Logger const&) = delete;
};
