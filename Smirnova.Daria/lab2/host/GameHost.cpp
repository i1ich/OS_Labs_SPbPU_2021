#include <sys/syslog.h>
#include <filesystem>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include <random>

#include "GameHost.h"
#include "../utils/GameRules.h"

namespace fs = std::filesystem;

GameHost* GameHost::s_host;

GameHost &GameHost::getInstance() {
    if (!s_host)
        s_host = new GameHost();
    return *s_host;
}

GameHost::GameHost() : m_clientInfo(0), m_isWork(false) {
    struct sigaction sig{};
    memset(&sig, 0, sizeof(sig));
    sig.sa_sigaction = signalHandler;
    sig.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &sig, nullptr);
    sigaction(SIGUSR1, &sig, nullptr);
    sigaction(SIGUSR2, &sig, nullptr);
    sigaction(SIGINT, &sig, nullptr);
}

bool GameHost::isWork() {
    return m_isWork;
}

void GameHost::openConnection() {
    syslog(LOG_INFO, "INFO: Start connect in host...");

    m_connection.open(getpid(), true);

    sem_unlink(GameRules::SEM_HOST_NAME.c_str());
    sem_unlink(GameRules::SEM_CLIENT_NAME.c_str());
    m_semaphoreHost = sem_open(GameRules::SEM_HOST_NAME.c_str(),  O_CREAT, 0666, 0);
    m_semaphoreClient = sem_open(GameRules::SEM_CLIENT_NAME.c_str(),  O_CREAT, 0666, 0);

    if (m_semaphoreHost == SEM_FAILED) {
        m_connection.close();
        throw std::runtime_error("host semaphore open failed with error " + std::string(strerror(errno)));
    }
    if (m_semaphoreClient == SEM_FAILED) {
        sem_close(m_semaphoreHost);
        m_connection.close();
        throw std::runtime_error("client semaphore open failed with error " + std::string(strerror(errno)));
    }
    syslog(LOG_INFO, "INFO: Connection in host is set");
}

int GameHost::riddleNum() {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(GameRules::MIN_WOLF_NUM, GameRules::MAX_WOLF_NUM);
    return uni(rng);
}

void GameHost::start() {
    Message msg;
    int wolfNum;
    m_isWork = true;
    m_clientInfo = ClientInfo(0);
    while (m_isWork){
        if (!m_clientInfo.is_ready){
            waitConnection();
        }
        else{
            wait();
            m_connection.read(&msg, sizeof(msg));
            wolfNum = riddleNum();
            syslog(LOG_INFO, "INFO: Wolf number: %i", wolfNum);
            updateStatus(msg, wolfNum);
            syslog(LOG_INFO, "INFO: New goat status: %s", (msg.is_goat_alive) ? "Alive" : "Dead");

            if(m_clientInfo.deaths >= GameRules::All_DEAD_END_TIMES){
                syslog(LOG_INFO, "INFO: Wolf win");
                return;
            }
            else{
                m_connection.write(&msg, sizeof(msg));
            }
            sem_post(m_semaphoreClient);
        }
    }
}

void GameHost::updateStatus(Message &answer, int rightNum) {
    bool isGoatStillAlive = true;
    if (answer.is_goat_alive) {
        if (std::abs(rightNum - answer.number) > GameRules::ALIVE_GOAT_SPACE_MUL) {
            m_clientInfo.deaths += 1;
            isGoatStillAlive = false;
        }
    } else {
        if (std::abs(rightNum - answer.number) > GameRules::DEAD_GOAT_SPACE_MUL) {
            m_clientInfo.deaths += 1;
            isGoatStillAlive = false;
        } else {
            m_clientInfo.deaths = 0;
        }
    }
    answer.is_goat_alive = isGoatStillAlive;
}

void GameHost::terminate() {
    m_isWork = false;
    if (m_clientInfo.is_ready) {
        kill(m_clientInfo.pid, SIGTERM);
        m_clientInfo = ClientInfo(0);
    }
    syslog(LOG_INFO, "INFO: terminate host");
    if (sem_unlink(GameRules::SEM_HOST_NAME.c_str()) == -1 ||
        sem_unlink(GameRules::SEM_CLIENT_NAME.c_str()) == -1) {
        throw std::runtime_error("sem_unlink error : " + std::string(strerror(errno)));
    }
    m_connection.close();
    syslog(LOG_INFO, "INFO: Host terminated");
}

void GameHost::wait() {
    struct timespec ts{};
    int rc = clock_gettime(CLOCK_REALTIME, &ts);
    if (rc == -1) {
        auto proc = "/proc/" + std::to_string(m_clientInfo.pid);
        if (fs::exists(proc)) {
            kill(m_clientInfo.pid, SIGTERM);
        }
        throw std::runtime_error("Can't get current time for sem_timedwait in host");
    }
    ts.tv_sec += GameRules::TIMEOUT;
    syslog(LOG_INFO, "INFO: Waiting for client...");
    if (sem_timedwait(m_semaphoreHost, &ts) == -1) {
        auto proc = "/proc/" + std::to_string(m_clientInfo.pid);
        if (fs::exists(proc)) {
            kill(m_clientInfo.pid, SIGTERM);
        }
        throw std::runtime_error("expired timeout in host");
    }
}

void GameHost::signalHandler(int signum, siginfo_t *info, void *ptr) {
    switch (signum) {
        case SIGUSR1:
            if (s_host->m_clientInfo.is_ready) {
                syslog(LOG_INFO, "INFO: Only one client is possible for host");
            } else {
                syslog(LOG_INFO, "INFO: Client with pid = %d connected to host", info->si_pid);
                s_host->m_clientInfo = ClientInfo(info->si_pid);
            }
            break;
        case SIGUSR2:
            syslog(LOG_INFO, "INFO: Host got sig about client error");
            s_host->m_clientInfo = ClientInfo(0);
            break;
        case SIGTERM:
        case SIGINT:
            if (s_host->m_clientInfo.is_ready) {
                kill(s_host->m_clientInfo.pid, signum);
                s_host->m_clientInfo = ClientInfo(0);
            }
            s_host->terminate();
            break;
        default:
            syslog(LOG_INFO, "INFO: Host got unknown signal");
            break;
    }
}

void GameHost::waitConnection() {
    syslog(LOG_INFO, "INFO: Waiting for client to connect...");
    wait();
    sem_post(m_semaphoreClient);
    syslog(LOG_INFO, "INFO: Client connected to host");
}
