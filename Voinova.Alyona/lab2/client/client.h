//
// Created by pikabol88 on 11/22/21.
//

#ifndef LAB2_CLIENT_H
#define LAB2_CLIENT_H

#include <csignal>
#include <semaphore.h>
#include <map>
#include "../conn/IConn.h"
#include "../stuff/ClientInfo.h"

class Client {
public:
    static  Client &getInstance();

    void run();
    void openConnection();
    void terminate();

    void setHostPid(int hostPid);

private:
    static Client _instance;

    IConn _connection;
    sem_t *_sem_host;
    sem_t *_sem_client;
    int _host_pid;

    bool _isRunning = true;

    explicit Client();
    int getWeather();
    static void handleSignal(int signum);

    int getWeather(const WeatherDTO &dto);

    Client(Client &) = delete;
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;
};


#endif //LAB2_CLIENT_H
