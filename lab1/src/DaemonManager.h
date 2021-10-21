#include <string>
#include <fstream>

class Daemon;

constexpr size_t cfgPathBufSize = 256;

class DaemonManager {
private:
    static const char* _pidFilename;
    static std::string _cfgPath;
    static Daemon* _daemon;

    static void writePID(std::ofstream& pidFile);
    static int readPID(std::ifstream& pidFile);
    static void signalHandler(int signal);
    static void findConfig();
public:
    static void createNewSession();
    static void setConfigPath(const char* path);
    static const char* getConfigPath();
};