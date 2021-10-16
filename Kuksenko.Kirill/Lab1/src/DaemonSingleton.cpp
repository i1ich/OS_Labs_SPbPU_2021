#include "DaemonSingleton.h"

DaemonSingleton::DaemonSingleton(std::string const& config) : daemon{ config } {}

DaemonSingleton::~DaemonSingleton() {}

DaemonSingleton* DaemonSingleton::instanse(std::string const& config) {
    static DaemonSingleton inst(config);

    return &inst;
}

Daemon& DaemonSingleton::value() {
    return daemon;
}
