#include "goat.h"

Goat::Goat(int pid) : _hostPid(pid) {
    signal(SIGUSR1, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
}


Goat::~Goat() {
    if (_semHost != SEM_FAILED) {
        sem_close(_semHost);
    }

    if (_semClient != SEM_FAILED) {
        sem_close(_semClient);
    }

    kill(_hostPid, SIGUSR2);

    if (!_conn.close()) {
        exit(errno);
    }

    syslog(LOG_INFO, "INFO_CLIENT: Goat been terminated");
}


bool Goat::openConnection() {
    syslog(LOG_INFO, "INFO_CLIENT: Start connect");

    if (!_conn.open(_hostPid, false)) {
        syslog(LOG_ERR, "ERROR_CLIENT: Connection not opened");
        return false;
    }

    _semHostName = "lab2_host";
    _semClientName = "lab2_client";

    if (!tryConnectToSem(&_semHost, _semHostName)) {
        syslog(LOG_ERR, "ERROR_CLIENT: Cannot connect host");
        return false;
    }

    if (!tryConnectToSem(&_semClient, _semClientName)) {
        syslog(LOG_ERR, "ERROR_CLIENT: Cannot connect client");
        sem_close(_semHost);
        return false;
    }

    syslog(LOG_INFO, "INFO_CLIENT: Connection is set");
    kill(_hostPid, SIGUSR1);

    return true;
}

void Goat::run() {
    syslog(LOG_INFO, "INFO_CLIENT: Goat's guessing number");

    Message message;

    GOAT_STATUS goatStatus = GOAT_STATUS::ALIVE;

    changeGoatNum(message, goatStatus);
    message.status = goatStatus;

    if (!_conn.write(&message, sizeof(message))) {
        syslog(LOG_ERR, "ERROR_CLIENT: cannot write conn");
        return;
    }

    if (sem_post(_semHost) == -1) {
        syslog(LOG_ERR, "ERROR_CLIENT: While sem_post( host )");
        return;
    }

    while (_isActiveGame) {

        if (!wait(_semClient)) {
            return;
        }

        if (!_conn.read(&message, sizeof(message))) {
            syslog(LOG_ERR, "ERROR_CLIENT: cannot read conn");
            return;
        }

        changeGoatNum(message, message.status);

        if(!_conn.write(&message, sizeof(message))) {
            syslog(LOG_ERR, "ERROR_CLIENT: cannot write conn");
            return;
        }

        if (sem_post(_semHost) == -1) {
            syslog(LOG_ERR, "ERROR_CLIENT: While sem_post( host )");
            return;
        }
    }
}

int Goat::getGoatNum(int maxValue) {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(GameDefinitions::MIN_GOAT_NUM, maxValue);
    return uni(rng);
}

void Goat::changeGoatNum(Message &message, GOAT_STATUS goatStatus) {
    int goatNumber;
    if(goatStatus == GOAT_STATUS::ALIVE) {
        goatNumber = getGoatNum(GameDefinitions::MAX_ALIVE_GOAT_NUM);
    }
    else {
        goatNumber = getGoatNum(GameDefinitions::MAX_DEAD_GOAT_NUM);
    }
    message.number = goatNumber;
}


bool Goat::wait(sem_t* sem) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += GameDefinitions::WAIT_TIMEOUT;
    if (sem_timedwait(sem, &ts) == -1) {
        syslog(LOG_ERR, "ERROR_CLIENT: Wait timeout");
        return false;
    }

    return true;
}


void Goat::signalHandler(int signal) {
    Goat &goat = getInstance(0);

    // TODO change logs

    switch (signal) {
        case SIGTERM:
        case SIGINT:
            syslog(LOG_INFO, "INFO_CLIENT: Receive signal to terminate goat");
            exit(EXIT_SUCCESS);
        case SIGUSR1:
            kill(goat._hostPid, SIGTERM);
            exit(EXIT_SUCCESS);
        default:
            syslog(LOG_INFO, "INFO_CLIENT: Unknown signal");
            break;
    }
}

Goat &Goat::getInstance(int hostPid) {
    static Goat goat(hostPid);
    return goat;
}

bool Goat::tryConnectToSem(sem_t **sem, const std::string& name) {
    for (int i = 0; i < GameDefinitions::RECONNECT_TIMEOUT; i++) {
        *sem = sem_open(name.c_str(), 0);
        if (*sem != SEM_FAILED) {
            return true;
        }
        sleep(1);
    }
    return false;
}










