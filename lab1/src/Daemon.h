#include <filesystem>
#include <memory>

class Daemon {
private:
    static const size_t _period = 20; // seconds
    static std::unique_ptr<Daemon> _instance;

    std::filesystem::path _folder1;
    std::filesystem::path _folder2;
    bool _termSignal = false;
    bool _needToReloadCfg = false;

    Daemon();
    Daemon(const Daemon&) = delete;

    void parseConfig(const char* configPath);
    bool makeJob();
public:
    static Daemon* getDaemon();

    void proceed();
    void stop();

    void queryConfigReload();
    
    ~Daemon() = default;
};