#include <fstream>
#include <syslog.h>
#include <sys/wait.h>
#include "Daemon.h"
#include "DaemonManager.h"


std::unique_ptr<Daemon> Daemon::_instance = nullptr;

Daemon* Daemon::getDaemon() {
    if (!_instance) {
        _instance.reset(new Daemon());
    }
    return _instance.get();
}

void Daemon::parseConfig(const char* configPath) {
    std::ifstream cfg(configPath);
    if (cfg.bad()) {
        throw std::runtime_error("Unable to read config file");
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
        throw std::runtime_error("Check config file");
    }
}

Daemon::Daemon() {
    parseConfig(DaemonManager::getConfigPath());
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
            parseConfig(DaemonManager::getConfigPath());
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