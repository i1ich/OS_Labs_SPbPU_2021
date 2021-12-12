//
// Created by oleg on 11.12.2021.
//

#include "goat.h"

Goat* Goat::instance = new Goat();

Goat* Goat::getInstance(pid_t hostPid) {
    Goat::instance->_hostPid = hostPid;
    return Goat::instance;
}

Goat::Goat(): _runGame(false), _hostPid(0) {
    signal(SIGTERM, signalHandler);
    signal(SIGUSR1, signalHandler);
    signal(SIGINT, signalHandler);
}

Goat::~Goat(){
    if(_clientSemaphore != SEM_FAILED) {
        sem_close(_clientSemaphore);
    }
    if(_hostSemaphore != SEM_FAILED){
        sem_close(_hostSemaphore);
    }
    if(!_conn.close()){
        syslog(LOG_ERR, "ERROR: can't close connection");
    }
    kill(_hostPid, SIGTERM);
}

bool Goat::init() {
    syslog(LOG_INFO, "INFO: start initialize client");

    if(!_conn.open(_hostPid, false)){
        syslog(LOG_ERR, "ERROR: client can't open connection");
        return false;
    }

    if((_hostSemaphore = connectToSem(HOST_SEMAPHORE)) == SEM_FAILED){
        syslog(LOG_ERR, "ERROR: can't connect to host semaphore");
        return false;
    }

    if((_clientSemaphore = connectToSem(CLIENT_SEMAPHORE)) == SEM_FAILED){
        syslog(LOG_ERR, "ERROR: can't connect to client semaphore");
        return false;
    }

    _runGame = true;

    syslog(LOG_INFO, "INFO: client successfully initialize");

    return true;
}

sem_t *Goat::connectToSem(const std::string name) {
    sem_t* semaphore;
    for (int i = 0; i < NUM_TIMES; i++) {
        semaphore = sem_open(name.c_str(), 0);
        if (semaphore != SEM_FAILED) {
            return semaphore;
        }
        sleep(1);
    }
    return SEM_FAILED;
}

void Goat::run(){
    int goatNumber = getGoatNumber();
    if(!sendGoatMessage(goatNumber, STATUS::ALIVE_GOAT)){
        syslog(LOG_ERR, "ERROR: can't send goat message");
        return;
    }
    while(_runGame){
        if(!stopClient()){
            syslog(LOG_ERR, "ERROR: can't stop client");
            return;
        }
        Message wolfMessage;
        if(!getWolfMessage(&wolfMessage)){
            syslog(LOG_ERR, "ERROR: can't get wolf message");
            return;
        }
        int goatNumber = getGoatNumber();
        if(!sendGoatMessage(goatNumber, wolfMessage.status)){
            syslog(LOG_ERR, "ERROR: can't send goat message");
            return;
        }
        _runGame = canContinue(wolfMessage.status);
    }
}

bool Goat::getWolfMessage(Message *message) {
    return _conn.read(message, sizeof(Message));
}

int Goat::getGoatNumber() {
    std::random_device rd;
    std::mt19937 mersenne(rd());
    mersenne();
    return (int)(mersenne() % 100 + 1);
}

bool Goat::sendGoatMessage(int goatNumber, STATUS status) {
    Message message = {status, goatNumber};
    return _conn.write(&message, sizeof(Message));
}

bool Goat::stopClient() {
    if(sem_post(_hostSemaphore) == -1){
        syslog(LOG_ERR, "ERROR: host semaphore can't continue");
        return false;
    }
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += WAIT_TIME;
    if(sem_timedwait(_clientSemaphore, &ts) == -1){
        syslog(LOG_ERR, "ERROR: client semaphore can't wait");
        return false;
    }
    return true;
}

void Goat::signalHandler(int signal) {
    switch(signal){
        case SIGTERM:
            instance->_runGame = false;
            exit(EXIT_SUCCESS);
        case SIGINT:
            syslog(LOG_INFO, "INFO: receive delete client request");
            exit(EXIT_SUCCESS);
        case SIGUSR1:
            syslog(LOG_INFO, "INFO: client end game");
            kill(instance->_hostPid, SIGTERM);
            exit(EXIT_SUCCESS);
        default:
            syslog(LOG_INFO, "INFO: unknow command");
    }
}

bool Goat::canContinue(STATUS status) {
    switch(status){
        case STATUS::ALIVE_GOAT:
            _deathCount = 0;
            return true;
        case STATUS::DEAD_GOAT:
            _deathCount++;
            if(_deathCount == 2){
                return false;
            }
            return true;
        default:
            syslog(LOG_ERR, "ERROR: invalid wolf message status");
            return false;
    }
}
