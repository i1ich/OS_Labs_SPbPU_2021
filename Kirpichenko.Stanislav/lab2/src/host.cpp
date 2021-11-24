#include <semaphore.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "conn_mmap.h"
#include "conn_pipe.h"
#include "conn_shm.h"
#include "host.h"
#include "client.h"

using namespace std;

#ifndef TYPE_CODE
static_assert(true, "IPC type is undefined");
#else
#define CONCAT(x, y) x##y
#define CONN_CONCAT(x) CONCAT(Conn, x)
#define CONN_CLASS CONN_CONCAT(TYPE_CODE)
#endif

Host::Host(Conn* connection, sem_t* hostSemaphore, sem_t* clientSemaphore) {
    _connection = connection;
    _hostSem = hostSemaphore;
    _clientSem = clientSemaphore;
}

void Host::readDate(tm& date) {
    cout << "Type date as " << Weather::dateFormat << " (example: " << put_time(&date, Weather::dateFormat) << ") to get temperature history or prediction:\n";
    string line;
    while (1) {
        getline(cin, line);
        if (line.empty()) {
            _fEndWork = true;
            break;
        }
        istringstream stream(line);
        stream >> get_time(&date, Weather::dateFormat);
        if (stream.fail()) {
            cout << "Wrong date format, enter again\n";
        }
        else {
            break;
        }
    }
}

void Host::proceed() {
    syslog(LOG_INFO, "Starting host work");
    Weather weather;
    tm date = { 0 };
    timespec time;
    date.tm_mon = 1;
    date.tm_year = 114;
    date.tm_mday = 25;
    readDate(date);
    while (!_fEndWork) {
        weather.date = date;
        _connection->write(weather);
        if (sem_post(_clientSem) < 0) {
            throw std::runtime_error("Host synchronization error");
        }
        if (clock_gettime(CLOCK_REALTIME, &time) < 0) {
            throw runtime_error("Unable to get system time");
        }
        time.tv_sec += maxWaitTime;
        if (sem_timedwait(_hostSem, &time) < 0) {
            if (errno == ETIMEDOUT) {
                throw runtime_error("Server wait time exceeded");
            }
            else {
                throw runtime_error("A synchronization error occured");
            }
        }
        weather = _connection->read();
        cout << "temp: " << setprecision(2) << weather.temp << " C\n";
        readDate(date);
    }
    syslog(LOG_INFO, "Ending host work due to empty line input");
    cout << "Ending host work due to empty line input\n";
}

void getSemaphores(sem_t*& hostSem, sem_t*& clientSem, bool fCreate) {
    int oFlag = O_RDWR;
    if (fCreate) {
        oFlag |= O_CREAT;
    }
    int fd = shm_open("host_semaphore", oFlag, S_IRWXU | S_IRWXG);
    if (fd < 0) {
        throw runtime_error("Unable to get host semaphore shared memory object");
    }
    if (fCreate) {
        ftruncate(fd, sizeof(sem_t));
    }
    hostSem = (sem_t*)mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    fd = shm_open("client_semaphore", oFlag, S_IRWXU | S_IRWXG);
    if (fd < 0) {
        throw runtime_error("Unable to get client semaphore shared memory object");
    }
    if (fCreate) {
        ftruncate(fd, sizeof(sem_t));
    }
    clientSem = (sem_t*)mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fCreate) {
        sem_init(hostSem, 1, 0);
        sem_init(clientSem, 1, 0);
    }
}

Conn* getConnection(bool fCreate) {
    int oFlag = O_RDWR;
    if (fCreate) {
        oFlag |= O_CREAT;
    }
    int fd = shm_open("connection", oFlag, S_IRWXU | S_IRWXG);
    if (fd < 0) {
        throw runtime_error("Unable to get connection shared memory object");
    }
    if (fCreate) {
        ftruncate(fd, sizeof(CONN_CLASS));
    }
    void* con_mem = mmap(0, sizeof(CONN_CLASS), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    Conn* connection = nullptr;
    if (fCreate) {
        connection = new (con_mem) CONN_CLASS();
    }
    else {
        connection = (Conn*)con_mem;
    }
    return connection;
}

int main() {
    sem_t* hostSem = nullptr;
    sem_t* clientSem = nullptr;
    Host* host = nullptr;
    Conn* connection = nullptr;
    pid_t pid = 0;
    try {
        openlog("Kirpichenko_lab2", LOG_PID | LOG_PERROR, LOG_USER);        
        getSemaphores(hostSem, clientSem, true);
        connection = getConnection(true);
        host = new Host(connection, hostSem, clientSem);

        pid = fork();
        if (pid == 0) {
            Client* client = nullptr;
            try {
                getSemaphores(hostSem, clientSem, false);
                client = new Client(getConnection(false), hostSem, clientSem);
                client->proceed();
            } catch (const exception& e) {
                syslog(LOG_ERR, "A client exception occured: %s", e.what());
            }
            delete client;
            return 0;
        }
        else {
            host->proceed();
        }
    } catch (const exception& e) {
        cout << "Aborting programm due to exception " << e.what() << endl;
        syslog(LOG_ERR, "A host exception occured: %s", e.what());
        
    }
    if (pid) {
        kill(pid, SIGTERM);
    }
    waitpid(0, nullptr, 0);
    shm_unlink("host_semaphore");
    shm_unlink("client_semaphore");
    if (connection) {
        connection->~Conn();
    }
    shm_unlink("connection");
    delete host;
    closelog();
    return 0;
}
