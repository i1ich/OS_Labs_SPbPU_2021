#include "client.h"

bool client::_isTerminate = false;

client* client::getInstance(int id, int seed)
{
    static client _client(id, seed);
    return &_client;
}

client::client(int id, int seed): _clientId(id), _randomInt(seed) {}

void client::getWeather(message &msg)
{
    msg.setTemp( _randomInt + msg.getDay() + msg.getMonth() + msg.getYear());
    return;
}

void client::setConnection(connection *c)
{
    _connection = c;
}

void client::setSem(sem_t* semC, sem_t* semH)
{
    _semaphoreClient = semC;
    _semaphoreHost = semH;
}

void client::run()
{
    syslog(LOG_INFO, "Client running");
    message msg;

    while (!_isTerminate)
    {
        std::cout<<"client with id " << _clientId << " is waiting\n";
        sem_wait(_semaphoreClient);

        _connection->read(&msg, sizeof(msg));
        std::cout<<"Client: got msg: d - " << msg.getDay() << " m  - "<<msg.getMonth()<<" y - "<<msg.getYear()<<std::endl;
        getWeather(msg);

        _connection->write(&msg, sizeof(msg));
        std::cout<<"Client " << _clientId <<": Temp = "<< msg.getTemp()<<std::endl;
        sem_post(_semaphoreHost);
    }

}

void client::signalHandler(int signal)
{
    if (signal == SIGTERM)
    {
        syslog(LOG_INFO, "Client has received SIGTERM signal");
        _isTerminate = true;
    }
}
