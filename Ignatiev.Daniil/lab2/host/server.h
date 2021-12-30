#ifndef SERVER_H
#define SERVER_H
#include <iomanip>
#include <iostream>
#include <semaphore.h>
#include <vector>
#include <fcntl.h>
#include <sstream>
#include <cstring>
#include <thread>
#include "../connection/connection.h"
#include "../connection/message.h"
#include "../host/client.h"


class server
{
public:

    server(int clientsNum);
    ~server(){};
    void start();

private:
    static bool _isTerminate;
    int _clientNum;
    int _hostPid;
    int _answersNum;
    std::vector<connection*> _conns;
    std::vector<sem_t*> _semaphoresHost;
    std::vector<std::string> _semHostNames;
    std::vector<std::thread*> _threads;

    std::vector<sem_t*> _clientSemaphores;
    std::vector<std::string> _semClientNames;
    std::vector<int> _clientPids;

    bool dateToMsg(std::string date, message &msg);
    void static signalHandler(int signum, siginfo_t *info, void *ptr);
    void static setTerminate();
    bool hostOpenConnection();
    void createClient(int id);
    void run();
    void listenForAnswer(int id);
    void terminate(int signum);

};
#endif
