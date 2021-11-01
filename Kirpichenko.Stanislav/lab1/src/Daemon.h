#pragma once
#include <filesystem>
#include <memory>

#include "Singleton.h"

class Daemon: public Singleton<Daemon> {
private:
    static const size_t _period = 20; // seconds

    std::filesystem::path _folder1;
    std::filesystem::path _folder2;
    
    bool _termSignal = false;
    bool _needToReloadCfg = false;

    Daemon();
    friend class Singleton<Daemon>;

    void parseConfig(const char* configPath);
    bool makeJob();
public:
    
    
    void proceed();
    void stop();

    void queryConfigReload();
    
    ~Daemon() = default;
};
