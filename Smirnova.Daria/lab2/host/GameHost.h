#ifndef LAB2_VAR5_GAMEHOST_H
#define LAB2_VAR5_GAMEHOST_H


#include <semaphore.h>
#include <csignal>
#include "../connections/Connection.h"
#include "../utils/Message.h"
#include "../utils/ClientInfo.h"

class GameHost {
public:
    static GameHost& getInstance();
    GameHost(const GameHost& host) = delete;
    GameHost& operator=(const GameHost&) = delete;

    void openConnection();
    void start();
    void terminate();
    bool isWork();

    ~GameHost();

private:
    GameHost();
    void updateStatus(Message& answer, int rightNum);
    void wait();
    int riddleNum();
    static void signalHandler(int signum, siginfo_t* info, void *ptr);
    void waitConnection();

    static GameHost* s_host;
    Connection* m_connection;
    ClientInfo m_clientInfo;
    sem_t* m_semaphoreClient;
    sem_t* m_semaphoreHost;
    bool m_isWork;
    int m_pid;
};

#endif //LAB2_VAR5_GAMEHOST_H
