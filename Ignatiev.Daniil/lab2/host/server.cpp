#include "../host/server.h"

bool server::_isTerminate = false;

void server::setTerminate()
{
    _isTerminate = true;
}

server::server(int clientsNum) : _clientNum(clientsNum)
{
    struct sigaction act{};
    memset(&act, 0, sizeof(act));
    act.sa_sigaction = signalHandler;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &act, nullptr);
    _hostPid = getpid();
    std::cout<<"Host started with pid " << _hostPid << std::endl;
    hostOpenConnection();
    _threads.clear();
}

bool server::hostOpenConnection()
{
    connection* conn;
    sem_t* semHost;
    for(int i = 0; i< _clientNum; i++)
    {
        conn = connection::createConnection();
        conn->open(i, true);
        _conns.push_back(conn);

        _semHostNames.push_back( std::string("semHost_") + std::to_string(i) );
        semHost = sem_open(_semHostNames[i].c_str(), O_CREAT, 0666, 0);
        if (semHost == SEM_FAILED)
        {
            syslog(LOG_ERR, "ERROR: handler: can`t open host semaphore error = %s", strerror(errno));
            return false;
        }
        _semaphoresHost.push_back(semHost);

        syslog(LOG_NOTICE, "handler: host semaphore created (%s)", _semHostNames[i].c_str());
    }

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
        c->setConnection(_conns[id]);
        c->setSem(semClient, _semaphoresHost[id]);

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

void server::listenForAnswer(int id)
{
    message ansMsg;
    sem_wait(_semaphoresHost[id]);
    _conns[id]->read(&ansMsg, sizeof(ansMsg));
    std::cout<<"Server: Answer from client " << id << ": "<< ansMsg.getTemp()<< std::endl;
    _answersNum++;
}

void server::run()
{
    message msg;
    std::string recievedDate;
    std::thread* thread;

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
                _answersNum = 0;
                std::cout<<"Server: data got.\n";
                std::cout<<"Server: sending data...\n";

                int id = 0;
                for(sem_t* sem : _clientSemaphores)
                {
                    _conns[id]->write(&msg, sizeof(msg));
                    sem_post(sem);
                    thread = new std::thread(&server::listenForAnswer, this, id);
                    _threads.push_back(thread);
                    id++;
                }

                //std::cout<<"Server: waiting for all cients.\n";
                while(_answersNum != _clientNum)    //waiting for all cients
                {;}
                //std::cout<<"Server: all responses got.\n";
                _threads.clear();
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
    int i = 0;
    for (auto c : _conns)
    {
        if (!c->close())
        {
            syslog(LOG_ERR, "Terminating error connection: %s", strerror(errno));
        }
        std::cout<<"Server: Connection " << i <<" closed\n";
        i++;
    }

    i = 0;
    for (auto s : _semaphoresHost)
    {
        if (s != SEM_FAILED)
        {
            s = SEM_FAILED;

            if (sem_unlink(_semHostNames[i].c_str()) != 0)
            {
                syslog(LOG_ERR, "Terminating error semHostName: %s", strerror(errno));
            }
        }
        i++;
    }

    i = 0;
    for(sem_t* s: _clientSemaphores)
    {
        if (s != SEM_FAILED)
        {
            s = SEM_FAILED;
            std::string _semClientName = "client_" + std::to_string(i);
            if (sem_unlink(_semClientName.c_str()) != 0)
            {
                syslog(LOG_ERR, "Terminating error semClientName: %s", strerror(errno));
            }
            std::cout<<"Server: Client " << i <<" semaphore unlinked\n";
        }
        i++;
    }

    _threads.clear();
}