#pragma once

#include "Daemon.h"

class DaemonSingleton {
private:
    Daemon daemon;

public:
    static DaemonSingleton* instanse(std::string const& config);

    Daemon& value();

private:
    DaemonSingleton(std::string const& config);
    ~DaemonSingleton();

    DaemonSingleton() = delete;
    DaemonSingleton(DaemonSingleton const& other) = delete;

};
 