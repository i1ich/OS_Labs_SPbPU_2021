#include <csignal>
#include <cstdlib>
#include <sys/syslog.h>
#include <random>
#include <filesystem>
#include <semaphore.h>
#include <stdexcept>
#include "Goat.h"
#include "../utils/GameRules.h"

namespace fs = std::filesystem;

Goat* Goat::s_goat;

Goat& Goat::getInstance(int hostPid) {
    if (!s_goat)
        s_goat = new Goat(hostPid);
    return *s_goat;
}

Goat::Goat(int hostPid) : m_isWork(false), m_isAlive(true), m_hostPid(hostPid) {
    signal(SIGUSR1, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
}

bool Goat::isWork() {
    return m_isWork;
}

void Goat::start() {
    syslog(LOG_INFO, "INFO: Start client");
    Message msg;
    m_isAlive = true;
    m_isWork = true;
    wait();
    syslog(LOG_INFO, "INFO: Host connected");

    msg.is_goat_alive = m_isAlive;

    while(m_isWork){
        msg.number = guessNum();
        syslog(LOG_INFO, "INFO: Goat number: %i", msg.number);
        m_connection.write(&msg, sizeof(Message));
        sem_post(m_semaphoreHost);
        wait();
        if(!m_isWork){
            syslog(LOG_INFO, "INFO: The host has finished working");
            continue;
        }
        m_connection.read(&msg, sizeof(Message));
        m_isAlive = msg.is_goat_alive;
        syslog(LOG_INFO, "INFO: Goat is alive: %i", m_isAlive);
    }
}

void Goat::openConnection() {
    syslog(LOG_INFO, "INFO: In client start connect...");

    m_connection.open(m_hostPid, false);

    m_semaphoreHost = sem_open(GameRules::SEM_HOST_NAME.c_str(), 0);
    m_semaphoreClient = sem_open(GameRules::SEM_CLIENT_NAME.c_str(), 0);

    if (m_semaphoreHost == SEM_FAILED) {
        m_connection.close();
        throw std::runtime_error("host semaphore open failed with error " + std::string(strerror(errno)));
    }
    if (m_semaphoreClient == SEM_FAILED){
        sem_close(m_semaphoreHost);
        m_connection.close();
        throw std::runtime_error("client semaphore open failed with error " + std::string(strerror(errno)));
    }

    syslog(LOG_INFO, "INFO: Connection in client is set");
    kill(m_hostPid, SIGUSR1);

}

int Goat::guessNum() {
    std::random_device rd;
    std::mt19937 rng(rd());
    int min = m_isAlive? GameRules::MIN_ALIVE_GOAT_NUM : GameRules::MIN_DEAD_GOAT_NUM;
    int max = m_isAlive? GameRules::MAX_ALIVE_GOAT_NUM : GameRules::MAX_DEAD_GOAT_NUM;
    std::uniform_int_distribution<int> uni(min, max);
    return uni(rng);
}

void Goat::wait() {
    struct timespec ts{};
    int rc = clock_gettime(CLOCK_REALTIME, &ts);
    if (rc == -1)
        throw std::runtime_error("Can't get current time for sem_timedwait in client");
    ts.tv_sec += GameRules::TIMEOUT;
    syslog(LOG_INFO, "INFO: Waiting for host to answer...");
    if (sem_timedwait(m_semaphoreClient, &ts) == -1)
        throw std::runtime_error("expired timeout in client");
}

void Goat::terminate() {
    auto proc = "/proc/" + std::to_string(m_hostPid);
    if (fs::exists(proc)) {
        kill(m_hostPid, SIGUSR2);
    }
    if (errno != 0)
        throw std::runtime_error("Client terminate failing with error : " + std::string(strerror(errno)));

    m_semaphoreHost = SEM_FAILED;
    m_semaphoreClient = SEM_FAILED;

    if (sem_close(m_semaphoreHost) == -1)
        throw std::runtime_error("Error in host sem_close : " + std::string(strerror(errno)));
    if (sem_close(m_semaphoreClient) == -1)
        throw std::runtime_error("Error in client sem_close : " + std::string(strerror(errno)));

    m_connection.close();
    m_isWork = false;
    syslog(LOG_INFO, "INFO: Goat has been terminated");
}

void Goat::signalHandler(int signal) {
    Goat &goat = getInstance(0);
    switch (signal) {
        case SIGTERM:
        case SIGINT:
            syslog(LOG_INFO, "INFO: Client got signal to terminate");
            exit(EXIT_SUCCESS);
        case SIGUSR1:
            kill(goat.m_hostPid, SIGTERM);
            exit(EXIT_SUCCESS);
        default:
            syslog(LOG_INFO, "INFO: Client got unknown signal");
            break;
    }
}
