#ifndef __DAEMON_H_
#define __DAEMON_H_


#include <string>
#include <vector>
#include <chrono>

enum EVENT_PERIOD {
    REPEAT_ONCE,
    REPEAT_HOUR,
    REPEAT_DAY,
    REPEAT_WEEK,
    REPEAT_MINUTE // for test
};

struct event {
    std::string text;
    EVENT_PERIOD period;
    //std::chrono::time_point<std::chrono::system_clock> startTime;
    std::tm startTime;
};

class daemon {
public:
    static bool runDaemon(const std::string& configFilePath = "");
    daemon(const daemon& d) = delete;
    daemon& operator=(const daemon& d) = delete;
private:
    static daemon* daemonInstance;
    static bool isRunned;
    std::string pidFilePath = "/var/run/lab1.pid";
    std::string configFileName;
    std::string initializePath;
    static const int timeStamp = 30;

    std::vector<event> events;

    void Idle(void);
    static void SignalHandler(int signal);
    void Execute(void);

    bool LoadConfig(const std::string& configFilePath = "");
    static bool SetPidFile(const std::string& filePath);
    bool KillOldByPid(void);

    void Notify(std::string text);
    bool CheckEvent(event ev);

    daemon(void) = default;
};


#endif //!__DAEMON_H_