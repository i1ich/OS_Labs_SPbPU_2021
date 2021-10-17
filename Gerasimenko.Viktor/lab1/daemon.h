#ifndef LAB_1_DAEMON_H
#define LAB_1_DAEMON_H

#include "definitions.h"
#include "parser.h"

class Daemon {
public:
    bool init(std::string const& configFile);

    void run();

    void stopDaemon();

    static Daemon& get();

private:
    Daemon() {}

    Daemon(Daemon const&) = delete;

    Daemon & operator=(Daemon const&) = delete;

    bool work();

    static void handleSignal(int signal);

    bool checkPid(pid_t pid);

    bool handlePid();

    bool setPid();

    std::string getFullPath(std::string const& path);

    bool loadConfig();

    void removeAllFilesFromDir(std::string const& dir);

    void moveFiles(std::string const& fromDir, std::string const& toDir);

    void moveFilesRecursively(std::string const& fromDir, std::string const& toDir,
                                     std::string const& pathToIMG, std::string const& pathToOTHERS);

    void makeDirWithCheck(std::string dirPath);

    bool exists(std::string const& path);

    bool isFile(std::string const& path);

    bool isDir(std::string const& path);

    const std::string& getFolderName(std::string const& path);


    static Daemon instance;

    std::string configFile_;
    std::string pidFile_;
    std::string homeDir_;

    int sleepTimeInterval_;
    std::string firstDir_;
    std::string secondDir_;

    bool workStatus_;

    std::string IMGFolder;
    std::string OTHERSFolder;



};

#endif //LAB_1_DAEMON_H
