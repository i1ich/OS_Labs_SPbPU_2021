#ifndef LAB_2_WOLF_H
#define LAB_2_WOLF_H

#include "../utils/definitions.h"
#include "../conn/conn.h"
#include "../conn/message.h"

class Wolf {

public:
    ~Wolf();

    static Wolf& getInstance(int hostPid);

    bool openConnection();

    void run();

private:
    Wolf(int hostsPid);

    Wolf(Wolf& w) = delete;

    Wolf& operator=(const Wolf& w) = delete;

    static void handleSignal(int sig, siginfo_t* sigInfo, void* ptr);

    bool wait(sem_t* sem);

    void changeGoatStatus(Message &message, int wolfNum);

    int getWolfNum();

    Conn _conn;

    int _hostPid;

    int _clientPid;

    sem_t* _semHost;

    sem_t* _semClient;

    std::string _semHostName;

    std::string _semClientName;

    int _goatDeathInRow;

    bool _isClientConnected;

    bool _isActiveGame = true;

};


#endif //LAB_2_WOLF_H
