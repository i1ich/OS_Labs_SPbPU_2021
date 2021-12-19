#include <sys/syslog.h>
#include <iostream>
#include <thread>
#include <algorithm>
#include <csignal>
#include <chrono>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

#include "host.h"
#include "../connections/connection.h"
#include "../common/commondf.h"

host* host::hostInstance = nullptr;

int main(int argc, char* argv[])
{
    openlog("lab2_host", LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_USER);

    try
    {
        host::Run();
    }
    catch (std::exception &err)
    {
        syslog(LOG_ERR, "An err %s", err.what());
    }
    try
    {
        host::Term();
    }
    catch (std::exception &err)
    {
        syslog(LOG_ERR, "An err %s", err.what());
    }
    closelog();
    return 0;
}

void host::signalHandler(int signum, siginfo_t* info, void *ptr)
{
    switch (signum)
    {
    case SIGUSR1:
    {
        syslog(LOG_INFO, "Client %d request connection to host", info->si_pid);
        int i = -1;
        while (!std::atomic_compare_exchange_strong(&(hostInstance->waitingClient), &i, (int)info->si_pid))
            ;
        std::thread session(host::ClientSession, info->si_pid);
        session.detach();
        hostInstance->waitingClient = -1;
        break;
    }
    case SIGTERM:
    {
        host::Term();
        break;
    }
    }
}

host::host(void) : waitingClient(-1)
{
  struct sigaction sig{};
  memset(&sig, 0, sizeof(sig));
  sig.sa_sigaction = signalHandler;
  sig.sa_flags = SA_SIGINFO;
  sigaction(SIGTERM, &sig, nullptr);
  sigaction(SIGUSR1, &sig, nullptr);
  //sigaction(SIGUSR2, &sig, nullptr);
  //sigaction(SIGINT, &sig, nullptr);
}

void host::Run(void)
{
    //pid_t hostPid = getpid();
    printf("host pid = %i\n", getpid());

    hostInstance = new host();
    printf("Input d to input date in dd.mm.yyyy format and string(max 128 chars) with details\nInput q to quit\n");

    while (true)
    {
        date input;
        bool dateInputed = false;
        char action = 0;
        scanf("%c", &action);
        if (action == 'q')
        {
            break;
        } else if (action == 'd')
        {
            scanf("%d.%d.%d %s", &input.day, &input.mon, &input.year, input.string);
            dateInputed = true;
        }

        if (dateInputed)
        {
            hostInstance->sessionsMutex.lock();
            for (auto &session: hostInstance->sessions)
            {
                SerializeDate(input, (char *) session.bulkToSend);
                session.needSend = true;
            }
            hostInstance->sessionsMutex.unlock();
            printf("waiting response from clients\n");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            hostInstance->sessionsMutex.lock();
            for (auto &session: hostInstance->sessions)
            {
                session.needSend = false;
                if (!session.needRead)
                {
                    printf("Client %i dont answer\n", (int)session.clientPid);
                } else
                {
                    weather w;
                    w = DeserializeWeather((char *) session.bulkToRead);
                    printf("Client %i answered: temperature is %i and %s\n", (int)session.clientPid,
                           w.temperature, w.string);
                }
            }
            hostInstance->sessionsMutex.unlock();
            printf("Input d to input date in dd.mm.yyyy format and string(max 128 chars) with details\nInput q to quit\n");
        }
    }
}

void host::Term(void)
{
    if (!hostInstance)
        return;

    hostInstance->sessionsMutex.lock();
    for (auto &session: hostInstance->sessions)
    {
        session.termClient = true;
        kill(session.clientPid, SIGTERM);
    }
    hostInstance->sessionsMutex.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    hostInstance->sessions.clear();
    delete hostInstance;
    hostInstance = nullptr;
    syslog(LOG_INFO, "Host terminated");
}

void host::ClientSession(pid_t clientPid)
{
    session_data *curSession;
    connection *curConnection;
    sem_t *semaphoreClientRead, *semaphoreHostRead;

    hostInstance->sessionsMutex.lock();
    if (std::find_if(hostInstance->sessions.begin(),
                     hostInstance->sessions.end(),
                     [clientPid](const session_data &data)
                     {
                         return data.clientPid == clientPid;
                     })
        != hostInstance->sessions.end())
    {
        syslog(LOG_INFO, "Client %d already connected", clientPid);
        hostInstance->sessionsMutex.unlock();
        return;
    }
    hostInstance->sessions.push_back(session_data());
    hostInstance->sessions.back().clientPid = clientPid;
    curSession = &hostInstance->sessions.back();
    hostInstance->sessionsMutex.unlock();

    std::string semNameHostRead = "/" + std::to_string(clientPid);
    std::string semNameClientRead = semNameHostRead + std::to_string(clientPid);
    semaphoreHostRead = sem_open(semNameHostRead.c_str(), O_CREAT | O_EXCL, 0777, 0);
    if (semaphoreHostRead == SEM_FAILED)
    {
        syslog(LOG_ERR, "Semaphore creation error");
        return;
    }
    semaphoreClientRead = sem_open(semNameClientRead.c_str(), O_CREAT | O_EXCL, 0777, 0);
    if (semaphoreClientRead == SEM_FAILED)
    {
        syslog(LOG_ERR, "Semaphore creation error");
        sem_close(semaphoreHostRead);
        return;
    }

    curConnection = connection::CreateConnection(clientPid, true);
    if (!curConnection)
    {
        sem_close(semaphoreHostRead);
        sem_close(semaphoreClientRead);
        syslog(LOG_ERR, "Connection creation error");
        return;
    }
    kill(curSession->clientPid, SIGUSR1);

    curConnection->Open(0, true);

    while (!curSession->termClient)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        if (curSession->needSend)
        {
            curConnection->Send(curSession->bulkToSend, session_data::BULK_SIZE);
            sem_post(semaphoreClientRead);
            std::this_thread::sleep_for(std::chrono::milliseconds(30)); // for client get semaphore

            sem_wait(semaphoreHostRead);
            curConnection->Get(curSession->bulkToRead, session_data::BULK_SIZE);
            curSession->needRead = true;
            curSession->needSend = false;
        }
    }

    sem_close(semaphoreHostRead);
    sem_close(semaphoreClientRead);
    curConnection->Close();
    delete curConnection;
}
