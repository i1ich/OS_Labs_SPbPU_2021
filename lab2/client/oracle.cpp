#include <string.h>
#include <unistd.h>
#include <sys/syslog.h>
#include <semaphore.h>
#include <fcntl.h>
#include <thread>

#include "oracle.h"
#include "../connections/connection.h"

oracle *oracle::instance = nullptr;

weather oracle::Generate(date d)
{
    weather rez = { 0 };

    int32_t dat = d.mon * 1000000 + d.day * 10000 + d.year + seed;


    rez.temperature = ((dat % 303030) - 1337) / 3.0;


        sprintf(rez.string, "blue blue sky");

    return rez;
}

void oracle::signalHandler(int signum, siginfo_t* info, void *Ptr)
{
    switch (signum)
    {
    case SIGUSR1:
    {
        instance->connected = true;
        break;
    }
    case SIGTERM:
    {
        instance->needTerm = true;
        break;
    }
    }
}

void oracle::Run(pid_t hostPid)
{
    kill(hostPid, SIGUSR1);
    instance = new oracle();
    while (!instance->connected)
        ;

    std::string semNameHostRead = "/" + std::to_string(getpid());
    std::string semNameClientRead = semNameHostRead + std::to_string(getpid());
    sem_t *semaphoreClientRead = sem_open(semNameClientRead.c_str(), 0);
    if (semaphoreClientRead == SEM_FAILED)
    {
        syslog(LOG_ERR, "Semaphore opening ERROR");
        return;
    }

    sem_t *semaphoreHostRead = sem_open(semNameHostRead.c_str(), 0);
    if (semaphoreHostRead == SEM_FAILED)
    {
        syslog(LOG_ERR, "Semaphore opening ERROR");
        sem_close(semaphoreClientRead);
        return;
    }

    connection *curConnection = connection::CreateConnection(getpid(), false);
    if (!curConnection)
    {
        syslog(LOG_ERR, "Connection creation ERROR");
        sem_close(semaphoreHostRead);
        sem_close(semaphoreClientRead);
        return;
    }
    curConnection->Open(hostPid, false);

    while (!instance->needTerm)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        date inDate;
        sem_wait(semaphoreClientRead);
        curConnection->Get((void *)&inDate, 256);
        if (inDate.day == 0 && inDate.mon == 0 && inDate.year == 0)
        {
            sem_post(semaphoreHostRead);
            continue;
        }
        weather rez = instance->Generate(inDate);
        curConnection->Send(&rez, 256);
        sem_post(semaphoreHostRead);
    }

    sem_close(semaphoreHostRead);
    sem_close(semaphoreClientRead);
    curConnection->Close();
    delete curConnection;
}

oracle::oracle(void) : connected(false), needTerm(false)
{
    srand(getpid());
    seed = rand() % 1337;
  struct sigaction sig{};
  memset(&sig, 0, sizeof(sig));
  sig.sa_sigaction = signalHandler;
  sig.sa_flags = SA_SIGINFO;
  sigaction(SIGTERM, &sig, nullptr);
  sigaction(SIGUSR1, &sig, nullptr);
  //sigaction(SIGUSR2, &sig, nullptr);
  //sigaction(SIGINT, &sig, nullptr);

}

