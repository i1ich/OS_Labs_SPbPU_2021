//
// Created by oleg on 11.12.2021.
//

#ifndef LAB2_GOAT_H
#define LAB2_GOAT_H

#include <string>
#include "../data/common_data.h"
#include "../conn/message.h"
#include "../conn/conn.h"

#define NUM_TIMES 10
#define WAIT_TIME 5

class Goat {
public:
    static Goat* getInstance(pid_t hostPid);
    bool init();
    void run();
    ~Goat();
private:
    Goat();
    Goat(Goat& w) = delete;
    Goat& operator=(const Goat& w) = delete;

    static void signalHandler(int signal);
    bool getWolfMessage(Message* message);
    int getGoatNumber();
    bool sendGoatMessage(int goatNumber, STATUS status);
    bool stopClient();
    sem_t *connectToSem(std::string name);
    bool canContinue(STATUS status);

    static Goat* instance;
    bool _runGame;
    int _deathCount{};
    pid_t _hostPid;
    sem_t* _hostSemaphore{};
    sem_t* _clientSemaphore{};
    Conn _conn{};
};


#endif //LAB2_GOAT_H
