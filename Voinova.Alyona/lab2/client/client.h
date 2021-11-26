//
// Created by pikabol88 on 11/22/21.
//

#ifndef LAB2_CLIENT_H
#define LAB2_CLIENT_H

#include <csignal>
#include <semaphore.h>
#include <map>
//#include <IConnection.h>

class Client {
public:
    Client(Client &) = delete;
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;

    static  Client &getInstance(int host_pid);

    void run();
    void openConnection();
    void terminate();

private:
    //IConnection connection;
    sem_t *sem_host;
    sem_t *sem_client;
    int host_pid;
    bool isValid = true;

    explicit Client(int host_pid);
    int getWeather();
    static void handleSignel(int signum);
};


#endif //LAB2_CLIENT_H
