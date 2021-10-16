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

public:
    Logger(std::string const& name);
    ~Logger();

    void log(LEVEL level, std::string const& message) const;
};
