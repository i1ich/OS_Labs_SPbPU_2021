//
// Created by pikabol88 on 12/3/21.
//
#include <semaphore.h>
#include <cstring>
#include "client.h"
#include <iostream>
#include <syslog.h>
#include <random>
#include "../stuff/Constants.h"

Client &Client::getInstance() {
    static Client instance;
    return instance;
}

Client::Client() {
    signal(SIGTERM, handleSignal);
    signal(SIGINT, handleSignal);
}

void Client::setHostPid(int hostPid) {
    this->_host_pid = hostPid;
}

void Client::openConnection() {
    syslog(LOG_INFO, "[INFO] try connect");

    _connection.openConn(_host_pid, false);
    _sem_host = sem_open(Constants::SEM_HOST_NAME.c_str(), 0);
    _sem_client = sem_open(Constants::SEM_CLIENT_NAME.c_str(), 0);

    if (_sem_host == SEM_FAILED || _sem_client == SEM_FAILED) {
        throw std::runtime_error("sem_open failed with error " + std::string(strerror(errno)));
    } else {
        kill(_host_pid, SIGUSR1);
    }

}


void Client::terminate() {
    kill(_host_pid, SIGUSR2);
    if (errno != 0) {
        throw std::runtime_error("disconnected error " + std::string(strerror(errno)));
    }
    _sem_host = SEM_FAILED;
    _sem_client = SEM_FAILED;
    if (sem_close(_sem_client) == -1 || sem_close(_sem_host) == -1) {
        throw std::runtime_error("sem_close error " + std::string(strerror(errno)));
    }
    _connection.closeConn();
}

void Client::run()  {
    timespec ts{};
    WeatherDTO dto;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += Constants::TIMEOUT;

    syslog(LOG_INFO, "[INFO] wait host connect");

    if (sem_timedwait(_sem_client, &ts) == -1) {
        throw std::runtime_error("timeout  " + std::string(strerror(errno)));
    }

    syslog(LOG_INFO, "[INFO] host connected");

    _isRunning = true;
    while (_isRunning) {
        syslog(LOG_INFO, "[INFO] wait host data");
        sem_wait(_sem_client);
        _connection.readConn(&dto, sizeof(dto));
        int weather = getWeather(dto);
        dto.setTemp(weather);
        _connection.writeConn(&dto, sizeof(dto));
        syslog(LOG_INFO, "[INFO] call host" );
        sem_post(_sem_host);
    }
}

int Client::getWeather(const WeatherDTO &dto) {
    long date = dto.getYear() * 10000 + dto.getMonth() * 100 + dto.getDay();
    std::minstd_rand generator(date);
    std::uniform_int_distribution<int> distribution(Constants::MIN_TEMPERATURE, Constants::MAX_TEMPERATURE);
    return distribution(generator);
}

void Client::handleSignal(int signum) {
    Client &client = getInstance();
    switch (signum) {
        case SIGTERM:
        case SIGINT:
            syslog(LOG_INFO, "[INFO] stop work");
            client._isRunning = false;
            break;
        default:
            break;
    }
}