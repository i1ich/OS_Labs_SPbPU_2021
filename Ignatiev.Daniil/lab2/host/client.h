#ifndef CLIENT_H
#define CLIENT_H

#include "../connection/message.h"
#include "../connection/connection.h"
#include <semaphore.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sys/types.h>
#include "../connection/connection.h"

class client
{
public:
    static client* getInstance(int id, int seed);
    void run();
    void setConnection(connection *c);
    void setSem(sem_t* semC, sem_t* semH);

private:
    connection *_connection;
    sem_t *_semaphoreHost;
    sem_t *_semaphoreClient;
    std::string _semClientName;
    std::string _semHostName;

    int _clientId;
    int _randomInt;
    static bool _isTerminate;

    client(int id, int seed);

    void getWeather(message &msg);
    static void signalHandler(int signum);
};

#endif //CLIENT_H
