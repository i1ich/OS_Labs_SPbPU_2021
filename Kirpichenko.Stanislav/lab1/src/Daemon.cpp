#include <fstream>
#include <syslog.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string>

#include "Daemon.h"
#include "DaemonManager.h"

void Daemon::parseConfig(const char* configPath) {
    std::ifstream cfg(configPath);
    if (cfg.bad()) {
        throw std::runtime_error(std::string("Unable to read config file ") + configPath);
    }
    std::string line;
    while (cfg.good() && line.empty()) {
        cfg >> line;
    }
    _folder1 = line;
    line.clear();
    while (cfg.good() && line.empty()) {
        cfg >> line;
    }
    _folder2 = line;
    if (!std::filesystem::is_directory(_folder1) || !std::filesystem::is_directory(_folder2)) {
        throw std::runtime_error(std::string("Check config file ") + configPath);
    }
    syslog(LOG_INFO, "Config file %s is loaded", configPath);
}

Daemon::Daemon() {
    parseConfig(Singleton<DaemonManager>::getInstance()->getConfigPath());
}

bool Daemon::makeJob() {
    try {
        std::filesystem::directory_iterator fol2it(_folder2);
        for (auto& content: fol2it) {
            std::filesystem::remove_all(content);
        }   
        std::filesystem::directory_iterator fol1it(_folder1);
        for (auto& content: fol1it) {
            if (content.path().extension() == ".bk") {
                std::filesystem::copy_file(content, _folder2 / content.path().filename());
            }
        }
    }
    catch (std::exception& e) {
        syslog(LOG_WARNING, "%s", e.what());
        return false;
    }
    return true;
}

void Daemon::proceed() {
    while (makeJob()) {
        syslog(LOG_INFO, "Job is done");
        sleep(_period);
        if (_termSignal) {
            _termSignal = false;
            return;
        }
        if (_needToReloadCfg) {
            _needToReloadCfg = false;
            parseConfig(Singleton<DaemonManager>::getInstance()->getConfigPath());
        }
    }
    syslog(LOG_WARNING, "Daemon ends its work due to occured error");
}

void Daemon::queryConfigReload() {
    _needToReloadCfg = true;
}

void Daemon::stop() {
    _termSignal = true;
}
