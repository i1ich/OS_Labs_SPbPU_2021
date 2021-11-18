#include "wolf.h"

Wolf::Wolf(int pid) : _hostPid(pid), _clientPid(0), _goatDeathInRow(0), _isClientConnected(false) {
    openlog("WolfGoatsGame", LOG_NDELAY | LOG_PID, LOG_USER);
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_flags = SA_SIGINFO;
    sig.sa_sigaction = handleSignal;
    sigaction(SIGTERM, &sig, nullptr);
    sigaction(SIGUSR1, &sig, nullptr);
    sigaction(SIGUSR2, &sig, nullptr);
    sigaction(SIGINT, &sig, nullptr);
}

Wolf::~Wolf() {
    if (_isClientConnected) {
        _isClientConnected = false;
        kill(_clientPid, SIGTERM);
    }

    if (_semHost != SEM_FAILED) {
        sem_unlink(_semHostName.c_str());
    }

    if (_semClient != SEM_FAILED) {
        sem_unlink(_semClientName.c_str());
    }

    if (!_conn.close()) {
        exit(errno);
    }

    syslog(LOG_INFO, "INFO_HOST: Wolf been terminated");
    closelog();
}

Wolf &Wolf::getInstance(int hostPid) {
    static Wolf wolf(hostPid);
    return wolf;
}

bool Wolf::openConnection() {
    syslog(LOG_INFO, "INFO_HOST: Start connection");

    if (!_conn.open(_hostPid, true)){
        syslog(LOG_ERR, "ERROR_HOST: Connection not opened");
        return false;
    }

    _semHostName = "lab2_host";
    _semClientName = "lab2_client";

    _semHost = sem_open(_semHostName.c_str(), O_CREAT, 0666, 0);
    if (_semHost == SEM_FAILED) {
        syslog(LOG_ERR, "ERROR_HOST: SemHost cannot opened");
        return false;
    }

    _semClient = sem_open(_semClientName.c_str(), O_CREAT, 0666, 0);
    if (_semClient == SEM_FAILED) {
        sem_unlink(_semHostName.c_str());
        syslog(LOG_ERR, "ERROR_HOST: SemClient cannot opened");
        return false;
    }

    syslog(LOG_INFO, "INFO_HOST: Connection is set");
    pause();

    return true;
}

void Wolf::run() {
    syslog(LOG_INFO, "INFO_HOST: Wolf's searching for goats");
    Message message;
    while (_isActiveGame) {

        if (!wait(_semHost)) {
            return;
        }

        syslog(LOG_INFO, "INFO_HOST: ---Game step---");

        if (!_conn.read(&message, sizeof(message))) {
            syslog(LOG_ERR, "ERROR_HOST: Cannot read conn");
            return;
        }

        int wolfNum = getWolfNum();

        changeGoatStatus(message, wolfNum);

        if (_goatDeathInRow >= GameDefinitions::MAX_GOAT_DEATH_IN_ROW) {
            syslog(LOG_INFO, "INFO_HOST: Wolf win");
            return;
        }

        if(!_conn.write(&message, sizeof(message))) {
            syslog(LOG_ERR, "ERROR_HOST: Cannot write conn");
            return;
        }

        if (sem_post(_semClient) == -1) {
            syslog(LOG_ERR, "ERROR_HOST: While sem_post( client )");
            return;
        }
    }
}

bool Wolf::wait(sem_t* sem) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += GameDefinitions::WAIT_TIMEOUT;
    if (sem_timedwait(sem, &ts) == -1) {
        syslog(LOG_ERR, "ERROR_HOST: Wait timeout");
        return false;
    }

    return true;
}

void Wolf::changeGoatStatus(Message &message, int wolfNum) {
    int goatNumber = message.number;
    GOAT_STATUS goatStatus = message.status;

    if (goatStatus == GOAT_STATUS::ALIVE && abs(goatNumber - wolfNum) > GameDefinitions::ALIVE_DEVIATION) {
        _goatDeathInRow += 1;
        goatStatus = GOAT_STATUS::DEAD;
    } else if (goatStatus == GOAT_STATUS::DEAD) {
        if (abs(goatNumber - wolfNum) > GameDefinitions::DEAD_DEVIATION) {
            _goatDeathInRow += 1;
        } else {
            _goatDeathInRow = 0;
            goatStatus = GOAT_STATUS::ALIVE;
        }
    }

    syslog(LOG_INFO, "INFO_HOST: Goat num is %i", goatNumber);
    syslog(LOG_INFO, "INFO_HOST: Wolf num is %i", wolfNum);
    syslog(LOG_INFO, "INFO_HOST: Goat status is %s", goatStatus == GOAT_STATUS::ALIVE ? "alive" : "dead");

    message.status = goatStatus;
}

int Wolf::getWolfNum() {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(GameDefinitions::MIN_WOLF_NUM, GameDefinitions::MAX_WOLF_NUM);
    return uni(rng);
}

void Wolf::handleSignal(int sig, siginfo_t *sigInfo, void *ptr) {
    static Wolf& wolf = getInstance(0);
    switch (sig) {
        case SIGUSR1:
            if (wolf._isClientConnected) {
                syslog(LOG_INFO, "INFO_HOST: Client has already connected");
            } else {
                syslog(LOG_INFO, "INFO_HOST: Client connected: pid is %d", sigInfo->si_pid);
                wolf._isClientConnected = true;
                wolf._clientPid = sigInfo->si_pid;
            }
            break;
        case SIGTERM:
        case SIGINT:
            syslog(LOG_INFO, "INFO_HOST: Receive signal to terminate wolf");
            exit(EXIT_SUCCESS);
        default:
            syslog(LOG_INFO, "INFO_HOST: Unknown signal");
            break;
    }
}



