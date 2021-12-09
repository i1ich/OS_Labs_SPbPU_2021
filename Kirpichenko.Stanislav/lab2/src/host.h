#pragma once
#include "client.h"

class Host {
    inline static unsigned int maxWaitTime = 5;
    bool _fEndWork = false;
    sem_t* _hostSem = nullptr;
    sem_t* _clientSem = nullptr;
    Conn* _connection = nullptr;
    void readDate(tm& date);
    public:
    Host(Conn* connection, sem_t* hostSemaphore, sem_t* clientSemaphore);
    void proceed();
    ~Host() = default;
};