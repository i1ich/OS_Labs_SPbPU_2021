#ifndef LAB1_DAEMON_H
#define LAB1_DAEMON_H

#include <string>
#include <sys/sendfile.h>
#include <fcntl.h>
using namespace std;

class daemon
{
public:
    static bool startDaemonization(const string& configFileParse);
private:
    Daemon(Daemon const&) = delete;
    Daemon & operator=(Daemon const&) = delete;
    static void clearDaemon();
    static void terminateDaemon();
    static string getFullPath(const string& path);
    static void loadConfig();
    static bool checkPidFile();
    static bool setPidFile();
    static void signalHandler(int signalNum);
    static void startWork();
    static void makeAction();
    static void moveFile(const char* pathSrc, const char* pathDst);
    static void dir1ToDir2();
    static void dir2ToDir1();


    static string configFile;
    static string pidFileName;
    static string dirHome;
    static string dir1;
    static string dir2;
    static unsigned int waitingTime;
    static unsigned int timeDifference;
};


#endif //LAB1_DAEMON_H
