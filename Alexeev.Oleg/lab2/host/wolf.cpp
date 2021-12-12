//
// Created by oleg on 11.12.2021.
//

#include "wolf.h"
#include "../client/goat.h"

Wolf* Wolf::instance = new Wolf();

Wolf* Wolf::getInstance() {
    return instance;
}

Wolf::Wolf(): _gameRun(false), _clientConn(false), _countDeath(0), _clientPid(0) {
    _pid = getpid();
    openlog("Game", LOG_NDELAY | LOG_PID, LOG_USER);
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_flags = SA_SIGINFO;
    sig.sa_sigaction = Wolf::signalHandle;
    sigaction(SIGTERM, &sig, nullptr);
    sigaction(SIGINT, &sig, nullptr);
}

Wolf::~Wolf() {
    if(_clientPid != 0) {
        kill(_clientPid, SIGTERM);
    }
    if(!_conn.close()){
        syslog(LOG_ERR, "ERROR: can't close connection");
    }
    if(_semHost != SEM_FAILED) {
        sem_unlink(HOST_SEMAPHORE);
    }
    if(_semClient != SEM_FAILED) {
        sem_unlink(CLIENT_SEMAPHORE);
    }
}

void Wolf::signalHandle(int sig, siginfo_t *sigInfo, void *ptr) {
    switch(sig){
        case SIGTERM:
            instance->_gameRun = false;
            return;
        case SIGINT:
            syslog(LOG_INFO, "INFO: wolf terminate");
            exit(EXIT_SUCCESS);
            return;
        default:
            syslog(LOG_INFO, "INFO: unknown command");
    }
}

bool Wolf::init() {
    syslog(LOG_INFO, "INFO: start initialize host");

    _semHost = sem_open(HOST_SEMAPHORE, O_CREAT, 0666, 0);
    if(_semHost == SEM_FAILED){
        syslog(LOG_ERR, "ERROR: can't create host semaphore");
        return false;
    }

    _semClient = sem_open(CLIENT_SEMAPHORE, O_CREAT, 0666, 0);
    if(_semClient == SEM_FAILED){
        sem_unlink(HOST_SEMAPHORE);
        syslog(LOG_ERR, "ERROR: can't create client semaphore");
        return false;
    }

    startClient(_pid);

    if(!_conn.open(_pid, true)){
        syslog(LOG_ERR, "ERROR: can't open connection");
        return false;
    }

    _gameRun = true;
    syslog(LOG_INFO, "INFO: host initialize successfully");
    return true;
}

bool Wolf::startClient(pid_t hostPid) {
    pid_t childPid = fork();
    if(childPid == 0) {
        _clientPid = getpid();
        Goat *goat = Goat::getInstance(hostPid);
        if (goat->init()) {
            goat->run();
        } else {
            syslog(LOG_ERR, "ERR: can't initialise client");
            return false;
        }
        exit(EXIT_SUCCESS);
    }
    return true;
}

void Wolf::run(){
    while(_gameRun){
        if(!stopHost()){
            syslog(LOG_ERR, "ERROR: can't stop wolf work");
            return;
        }
        Message goatMessage;
        if(!getGoatMessage(&goatMessage)){
            syslog(LOG_ERR, "ERROR: can't get goat message");
            return;
        }
        int wolfNumber = getWolfNumber();
        STATUS messageStatus = getNewStatus(wolfNumber, goatMessage);
        if(!sendWolfMessage(messageStatus, wolfNumber)){
            syslog(LOG_ERR, "ERROR: can't send wolf message");
            return;
        }
        _gameRun = checkRun(messageStatus);
        if(!_gameRun){
            kill(_clientPid, SIGTERM);
            _clientPid = 0;
        }
    }
}

bool Wolf::stopHost() {
    if(sem_post(_semClient) == -1){
        syslog(LOG_ERR, "ERROR: can't continue client work");
        return false;
    }
    if(sem_wait(_semHost) == -1){
        syslog(LOG_ERR, "ERROR: can't stop host work");;
        return false;
    }
    return true;
}

bool Wolf::getGoatMessage(Message *message) {
    return _conn.read(static_cast<void *>(message), sizeof(Message));
}

int Wolf::getWolfNumber() {
    std::random_device rd;
    std::mt19937 mersenne(rd());
    mersenne();
    return (int)(mersenne() % 100 + 1);
}

STATUS Wolf::getNewStatus(int wolfNumber, Message goatMessage) {
    int div = (int)std::abs(wolfNumber - goatMessage.number);
    switch(goatMessage.status){
        case STATUS::ALIVE_GOAT:
            if(div > 70){
                return STATUS::DEAD_GOAT;
            }
            else{
                return STATUS::ALIVE_GOAT;
            }
        case STATUS::DEAD_GOAT:
            if(div > 20){
                return STATUS::DEAD_GOAT;
            }
            else{
                return STATUS::ALIVE_GOAT;
            }
        default:
            return STATUS::ERROR;
    }
}

bool Wolf::sendWolfMessage(STATUS status, int wolfNumber) {
    Message message = {status, wolfNumber};
    return _conn.write(&message, sizeof(Message));
}

bool Wolf::checkRun(STATUS status) {
    if(status == STATUS::ALIVE_GOAT){
        _countDeath = 0;
        return true;
    }
    if(status == STATUS::DEAD_GOAT){
        _countDeath++;
        if(_countDeath == 2){
            syslog(LOG_INFO, "INFO: goat dead 2 game steps. Game over");
            return false;
        }
        return true;
    }
    syslog(LOG_ERR, "ERROR: error game status");
    return false;
}
