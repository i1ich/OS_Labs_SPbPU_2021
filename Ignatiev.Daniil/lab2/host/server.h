#ifndef SERVER_H
#define SERVER_H
#include <iomanip>
#include <iostream>
#include <semaphore.h>
#include <vector>
#include <fcntl.h>
#include<sstream>
#include <cstring>
#include "../connection/connection.h"
#include "../connection/message.h"
#include "../host/client.h"


class server
{
public:

    server();
    ~server(){};
    void start();
    static server* getInstance();
    void setNumOfClients(int n);

private:
    static bool _isTerminate;
    int _clientNum;
    int _hostPid;
    connection* _conn;
    sem_t* _semaphoreHost;
    std::string _semHostName;

    //std::vector<pthread_t> _clientThreads;
    std::vector<sem_t*> _clientSemaphores;
    std::vector<std::string> _semClientNames;
    std::vector<int> _clientPids;

    bool dateToMsg(std::string date, message &msg);
    void static signalHandler(int signum, siginfo_t *info, void *ptr);
    void static setTerminate();
    bool hostOpenConnection();
    void createClient(int id);
    void run();
    void terminate(int signum);

};
#endif
