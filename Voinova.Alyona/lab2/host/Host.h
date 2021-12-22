//
// Created by pikabol88 on 11/22/21.
//

#ifndef LAB2_HOST_H
#define LAB2_HOST_H

#include <semaphore.h>
#include <csignal>
#include <string>
#include "../conn/IConn.h"
#include "../stuff/ClientInfo.h"
#include "../data/WeatherDTO.h"

class Host {
public:
    static Host& getInstance();

    void openConn();

    void start();

    void terminate();


private:
    const std::string _month = "month";
    const std::string _day = "day";
    const std::string _year = "year";

    static Host _instance;

    sem_t *_semaphore_host;
    sem_t *_semaphore_client;
    IConn _connection;
    ClientInfo _clientInfo;

    bool _isRunning = false;

    static void handleSignal(int signum, siginfo_t *info, void *ptr);

    void  printWeatherForDate(WeatherDTO &data);

    WeatherDTO getDate();

    unsigned int getDataFromUser(int end, const std::string &type);

    void exit(int signum);

    Host();
    Host(Host &) = delete;
    Host(const Host &) = delete;
    Host &operator=(const Host &) = delete;
};


#endif //LAB2_HOST_H
