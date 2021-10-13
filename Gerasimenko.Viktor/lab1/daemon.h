#ifndef LAB_1_DAEMON_H
#define LAB_1_DAEMON_H

#include "definitions.h"
#include "parser.h"

class Daemon {
public:
    static bool init(std::string const& configFile);

    static void run();

    static void stopDaemon();

private:
    static bool work();

    static void signalHandler(int signal);

    static bool checkPid(pid_t pid);

    static bool handlePid();

    static bool setPid();

    static std::string getFullPath(std::string const& path);

    static bool loadConfig();

    static void removeAllFilesFromDir(std::string const& dir);

    static void moveFiles(std::string const& fromDir, std::string const& toDir);

    static void moveFilesRecursively(std::string const& fromDir, std::string const& toDir,
                                     std::string const& pathToIMG, std::string const& pathToOTHERS);

    static bool exists(std::string const& path);

    static bool isFile(std::string const& path);

    static bool isDir(std::string const& path);

    static const std::string& getFolderName(std::string const& path);

    static std::string configFile_;
    static std::string pidFile_;
    static std::string homeDir_;

    static int sleepTimeInterval_;
    static std::string firstDir_;
    static std::string secondDir_;

    static bool workStatus_;

    static std::string IMGFolder;
    static std::string OTHERSFolder;

};

#endif //LAB_1_DAEMON_H
