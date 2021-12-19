//
// Created by oleg on 11.12.2021.
//

#ifndef LAB2_WOLF_H
#define LAB2_WOLF_H

#include "../data/common_data.h"
#include "../conn/message.h"
#include "../conn/conn.h"


class Wolf {
public:
    static Wolf* getInstance();

    void run();
    bool init();

    ~Wolf();
private:
    Wolf();
    Wolf(Wolf& w) = delete;
    Wolf& operator=(const Wolf& w) = delete;

    bool stopHost();
    int getWolfNumber();
    bool getGoatMessage(Message* message);
    bool sendWolfMessage(STATUS status, int wolfNumber);
    STATUS getNewStatus(int wolfNumber, Message goatMessage);
    bool checkRun(STATUS status);
    bool startClient(pid_t hostPid);
    static void signalHandle(int sig, siginfo_t *sigInfo, void *ptr);

    static Wolf* instance;
    bool _gameRun;
    int _countDeath;
    sem_t* _semHost{};
    sem_t* _semClient{};
    pid_t _pid;
    pid_t _clientPid;
    Conn* _conn;
};


#endif //LAB2_WOLF_H
