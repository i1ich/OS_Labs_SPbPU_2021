#include <utility>
#include <cstdlib>
#include <stdexcept>
#include <syslog.h>
#include <signal.h>
#include <time.h>
#include "client.h"

bool Client::_fEndWork = false;

Client::Client(Conn* connection, sem_t* hostSemaphore, sem_t* clientSemaphore) {
    _connection = connection;
    _hostSem = hostSemaphore;
    _clientSem = clientSemaphore;
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = Client::signalHandler;
    sa.sa_flags = 0;
    if (sigaction(SIGTERM, &sa, 0) == -1) {
        throw std::runtime_error("Unable to set signals handler");
    }
    _randomizer = clock() % 2048;
}

int Client::getHash(const tm& temp) {
    int hash = (temp.tm_mday << (4 + 8)) | (temp.tm_mon << 8) | (temp.tm_year & 0xFF);
    return hash;
}

double Client::getTemp(const tm& date) {
    constexpr std::pair<int, int> monthTempBounds[] = {{-35, 5}, {-40, 5}, {-20, 10}, {-10, 20}, {0, 30}, {10, 30}, {15, 40}, {15, 35}, {10, 30}, {0, 20}, {-15, 15}, {-25, 10}};
    srand(getHash(date) ^ _randomizer);
    double normalized_rand = (double)rand() / RAND_MAX;
    auto& bounds = monthTempBounds[date.tm_mon];
    return normalized_rand * (bounds.second - bounds.first) + bounds.first;
}

void Client::proceed() {
    syslog(LOG_INFO, "Starting client work");
    Weather weather = {0};
    if (sem_wait(_clientSem) < 0) {
        if (errno == EINTR) {
            syslog(LOG_INFO, "Client wait was interrupted due to signal");
        }
        else {
            throw std::runtime_error("Client synchronization error");
        }
    }
    while (!_fEndWork) {
        weather = _connection->read();
        weather.temp = getTemp(weather.date);
        _connection->write(weather);
        sem_post(_hostSem);
        if (sem_wait(_clientSem) < 0) {
            if (errno == EINTR) {
                syslog(LOG_INFO, "Client wait was interrupted due to signal");
            }
            else {
                throw std::runtime_error("Client synchronization error");
            }
        }
    }
    syslog(LOG_INFO, "Client is ending its work");
}

void Client::signalHandler(int signal) {
    if (signal == SIGTERM) {
        syslog(LOG_INFO, "Client has received SIGTERM signal");
        Client::endWork();
    }
}

void Client::endWork() {
    _fEndWork = true;
}
