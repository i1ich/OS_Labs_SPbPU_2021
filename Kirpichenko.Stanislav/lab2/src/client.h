#pragma once
#include <semaphore.h>
#include "conn_interface.h"

class Client {
    static bool _fEndWork;
    sem_t* _hostSem = nullptr;
    sem_t* _clientSem = nullptr;
    Conn* _connection = nullptr;
    int _randomizer = 0;

    int getHash(const tm& date);
    double getTemp(const tm& date);
    static void signalHandler(int signal);
    public:
    Client(Conn* connection, sem_t* hostSemaphore, sem_t* clientSemaphore);
    void proceed();
    static void endWork();
    ~Client() = default;
};