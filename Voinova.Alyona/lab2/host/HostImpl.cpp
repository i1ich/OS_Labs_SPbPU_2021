//
// Created by pikabol88 on 11/27/21.
//
#include "Host.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <syslog.h>
#include "../stuff/Constants.h"



Host &Host::getInstance() {
    static Host instance;
    return instance;
}

Host::Host(): _clientInfo(0) {

    struct sigaction act = {0};
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handleSignal;

    if(sigaction(SIGTERM, &act, nullptr) == -1) {
        throw std::runtime_error("sigaction error " + std::string(strerror(errno)));
    }
    if(sigaction(SIGUSR1, &act, nullptr) == -1) {
        throw std::runtime_error("sigaction error " + std::string(strerror(errno)));
    }
    if(sigaction(SIGUSR2, &act, nullptr) == -1) {
        throw std::runtime_error("sigaction error " + std::string(strerror(errno)));
    }
    if(sigaction(SIGINT, &act, nullptr) == -1) {
        throw std::runtime_error("sigaction error " + std::string(strerror(errno)));
    }
}

void Host::openConn() {
    syslog(LOG_INFO, "[INFO] open connection");
    _connection.openConn(getpid(), true);
    _semaphore_host = sem_open(Constants::SEM_HOST_NAME.c_str(), O_CREAT, 0666, 0);
    if (_semaphore_host == SEM_FAILED ) {
        throw std::runtime_error("sem_open error " + std::string(strerror(errno)));
    }
    _semaphore_client = sem_open(Constants::SEM_CLIENT_NAME.c_str(), O_CREAT, 0666, 0);
    if (_semaphore_client == SEM_FAILED) {
        throw std::runtime_error("sem_open error " + std::string(strerror(errno)));
    }
}
void Host::start() {
    timespec ts{};
    WeatherDTO dto;

    syslog(LOG_INFO, "[INFO] wait client connect");

    pause();

    syslog(LOG_INFO, "[INFO] client attached");
    _isRunning = true;
    sem_post(_semaphore_client);
    while (_isRunning) {
        if (!_clientInfo.isConnected()) {
            syslog(LOG_INFO, "[INFO] wait client connect");
            while (!_clientInfo.isConnected()) {
                pause();
            }
            if (!_isRunning) {
                kill(_clientInfo.getPid(), SIGTERM);
                _clientInfo = ClientInfo(0);
                return;
            }
            syslog(LOG_INFO, "[INFO] client attached");
            sem_post(_semaphore_client);
        } else {
            dto = getDate();
            if (_clientInfo.isConnected()){
                _connection.writeConn(&dto, sizeof(dto));
            } else {
                continue;
            }
            syslog(LOG_INFO, "[INFO] call client");
            sem_post(_semaphore_client);
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += Constants::TIMEOUT;
            syslog(LOG_INFO, "[INFO] wait client data");
            if (sem_timedwait(_semaphore_host, &ts) == -1) {
                syslog(LOG_INFO, "[INFO] client timeout");
                if (errno == EINTR) {
                    continue;
                }
                syslog(LOG_INFO, "[INFO] try turn off client");
                kill(_clientInfo.getPid(), SIGTERM);
                _clientInfo = ClientInfo(0);
                continue;
            }
            syslog(LOG_INFO, "[INFO] execute");
            _connection.readConn(&dto, sizeof(dto));
            printWeatherForDate(dto);
        }
    }
}

void Host::terminate() {
    _isRunning = false;
    _semaphore_client = SEM_FAILED;
    _semaphore_host = SEM_FAILED;
    if (sem_unlink(Constants::SEM_HOST_NAME.c_str()) == -1 ||
        sem_unlink(Constants::SEM_CLIENT_NAME.c_str()) == -1) {
        throw std::runtime_error("sem_unlink error " + std::string(strerror(errno)));
    }
    _connection.closeConn();
}

void Host::printWeatherForDate(WeatherDTO &data) {
    std::cout << data.getDay() << "." << data.getMonth() << "." << data.getYear() << " : " << data.getTemp() << std::endl;
}

void Host::handleSignal(int signum, siginfo_t *info, void *ptr) {
    static Host& host = getInstance();

    switch (signum) {
        case SIGUSR1:
            if (host._clientInfo.isConnected()){
                syslog(LOG_INFO, "[INFO] client has been already connected");
            } else {
                syslog(LOG_INFO, "[INFO] attaching client with pid = %i" ,info->si_pid);
                host._clientInfo = ClientInfo(info->si_pid);
            }
            break;
        case SIGUSR2:
            syslog(LOG_INFO, "[INFO] disconnect client");
            if(host._clientInfo.getPid() == info->si_pid){
                host._clientInfo = ClientInfo(0);
            }
            break;
        case SIGTERM:
        case SIGINT:
            if (host._clientInfo.isConnected()) {
                kill(host._clientInfo.getPid(), signum);
                host._clientInfo = ClientInfo(0);
            }
            host.terminate();
            break;
        default:
            syslog(LOG_INFO, "[INFO] unknown signal");
            break;
    }
}

WeatherDTO Host::getDate() {
    bool result = false;
    unsigned int day;
    unsigned int month;
    unsigned int year;

    while (!result && _isRunning) {
        tm date{};
        day = getDataFromUser(31, _day);
        month = getDataFromUser(12, _month);
        year = getDataFromUser(9999, _year);
        date.tm_year = year - 1900;
        date.tm_mon = month - 1;
        date.tm_mday = day;
        date.tm_isdst = -1;
        tm tCheck = date;
        mktime(&tCheck);
        if ((date.tm_year != tCheck.tm_year || date.tm_mon != tCheck.tm_mon || date.tm_mday != tCheck.tm_mday) && _isRunning) {
            result = false;
            std::cout << "please enter correct date" << std::endl;
        } else {
            result = true;
        }
    }
    return WeatherDTO(day, month, year);
}

unsigned int Host::getDataFromUser(int end, const std::string &type) {
    int data = -1;
    std::string buf;
    while ((data < 0 || data > end) && _isRunning) {
        std::cout << "please enter number of " << type << "( 0 < number <= " << end << " )" << std::endl;
        std::getline(std::cin, buf,'\n');
        if(buf.compare(Constants::EXIT_CODE) == 0) {
            getInstance().exit(SIGTERM);
            return 0;
        }
        try {
            data = std::stoi(buf);
        }
        catch (const std::invalid_argument &exp) {
            std::cout << "not int input" << std::endl;
            std::cin.clear();
        }
    }
    if (data < 0) {
        data = 0;
    }
    return data;
}

void Host::exit(int signum) {
    static Host& host = getInstance();
    syslog(LOG_INFO, "[INFO] stop work");
    if (host._clientInfo.isConnected()) {
        kill(host._clientInfo.getPid(), signum);
        host._clientInfo = ClientInfo(0);
    }
    _isRunning = false;
}