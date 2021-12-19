#include "../host/server.h"

bool server::_isTerminate = false;

void server::setTerminate()
{
    _isTerminate = true;
}

server::server()
{
    struct sigaction act{};
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = signalHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
    _hostPid = getpid();
    std::cout<<"Host started with pid " << _hostPid << std::endl;
    hostOpenConnection();
}

void server::setNumOfClients(int n)
{
    _clientNum = n;
}

bool server::hostOpenConnection()
{
    _conn = connection::createConnection();
    _conn->open(0, true);
    _semHostName = "semHost";
    _semaphoreHost = sem_open(_semHostName.c_str(), O_CREAT, 0666, 0);
    if (_semaphoreHost == SEM_FAILED)
    {
        syslog(LOG_ERR, "ERROR: handler: can`t open host semaphore error = %s", strerror(errno));
        return false;
    }
    syslog(LOG_NOTICE, "handler: host semaphore created (%s)", _semHostName.c_str());
    return true;
}

void server::createClient(int id)
{

    std::string semClientName = "client_" + std::to_string(id);
    sem_t *semClient = sem_open(semClientName.c_str(), O_CREAT, 0666, 0);

    if (semClient == SEM_FAILED)
    {
        syslog(LOG_ERR, "ERROR: can`t open client semaphore error = %s", strerror(errno));
    }

    syslog(LOG_NOTICE, "client semaphore created (%s)", semClientName.c_str());
    _clientSemaphores.push_back(semClient);
    _semClientNames.push_back(semClientName);

    int seed = rand() % 50 - 25;
    int clientPid = fork();
    if (clientPid == 0)
    {
        std::cout<< "Created client with id " << id << " and pid " << getpid() << "\n";
        client *c = client::getInstance(id, seed);
        c->setConnection(_conn);
        c->setSem(semClient, _semaphoreHost);

        syslog(LOG_INFO, "Client with id %i created", id);
        c->run();
    }
    else
    {
        _clientPids.push_back(clientPid);
    }

}

bool server::dateToMsg(std::string input, message &msg)
{
    struct std::tm date = {};
    std::istringstream dateString(input);
    dateString >> std::get_time(&date, "%d/%m/%Y");
    if (dateString.fail())
    {
        std::cout << "Wrong date format, enter again\n";
        return false;
    }
    msg.setDay(date.tm_mday);
    msg.setMonth(date.tm_mon);
    msg.setYear(date.tm_year);
    //std::cout<<"Server: Data correct\n";
    //std::cout<<msg.getDay()<<" / "<<msg.getMonth()<<" / "<<msg.getYear()<<"\n";
    return true;
}

void server::start()
{
    syslog(LOG_INFO, "Server: Creating clients started");
    for (int id = 0; id < _clientNum; id++)
    {
        createClient(id);
    }
    run();
}

void server::run()
{
    message msg, ansMsg;
    std::string recievedDate;


    while (!_isTerminate)
    {
            std::cout << "Enter the date in format: Day/Month/Year OR enter q for exit."<< std::endl;
            std::cin >> recievedDate;

            if (recievedDate == "q" || recievedDate == "Q")
            {
                syslog(LOG_INFO, "Terminating server...");
                _isTerminate = true;
            }
            else if(dateToMsg(recievedDate, msg))
            {
                std::cout<<"Server: data got.\n";

                for(sem_t* sem : _clientSemaphores)
                {
                    _conn->write(&msg, sizeof(msg));
                    std::cout<<"Server: sending data...\n";
                    sem_post(sem);
                    sem_wait(_semaphoreHost);
                    _conn->read(&ansMsg, sizeof(ansMsg));
                    std::cout<<"Server: Answer from client: " << ansMsg.getTemp() << "\n";
                }
            }
            else
            {
                std::cout << "Server: Incrorrect format: try again!" << std::endl;
            }

            recievedDate = "";

    }

    std::cout<<"Terminating\n";
    terminate(SIGTERM);
}

void server::signalHandler(int signum, siginfo_t *info, void *ptr)
{
    switch (signum)
    {
        case SIGTERM:
        {
            setTerminate();
            break;
        }
    }
}

void server::terminate(int signum)
{
    syslog(LOG_INFO, "Starting clients terminating...");

    for (auto &p : _clientPids)
    {
        std::cout<<"Server: terminating client with pid "<< p << std::endl;
        kill(p, SIGKILL);
    }

    if (!_conn->close())
    {
        syslog(LOG_ERR, "Terminating error connection: %s", strerror(errno));
    }

    if (_semaphoreHost != SEM_FAILED)
    {
        _semaphoreHost = SEM_FAILED;

        if (sem_unlink(_semHostName.c_str()) != 0)
        {
            syslog(LOG_ERR, "Terminating error semHostName: %s", strerror(errno));
        }
    }

    int id = 0;

    for(sem_t* s: _clientSemaphores)
    {
        if (s != SEM_FAILED)
        {
            s = SEM_FAILED;
            std::string _semClientName = "client_" + std::to_string(id);
            if (sem_unlink(_semClientName.c_str()) != 0)
            {
                syslog(LOG_ERR, "Terminating error semClientName: %s", strerror(errno));
            }
            std::cout<<"Server: Client " << id <<" semaphore unlinked\n";
        }
        id++;
    }

}