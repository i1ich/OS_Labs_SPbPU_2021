#ifndef LAB_2_GOAT_H
#define LAB_2_GOAT_H

#include "../utils/definitions.h"
#include "../conn/conn.h"
#include "../conn/message.h"

class Goat {

public:
    ~Goat();

    static Goat& getInstance(int hostPid);

    bool openConnection();

    void run();


private:
    Goat(int pid);

    Goat(Goat& w) = delete;

    Goat& operator=(const Goat& w) = delete;

    bool tryConnectToSem(sem_t** sem, const std::string& name);

    static void signalHandler(int signal);

    int getGoatNum(int maxValue);

    void changeGoatNum(Message &message, GOAT_STATUS goatStatus);

    bool wait(sem_t* sem);

    Conn _conn;

    int _hostPid;

    sem_t* _semHost;

    sem_t* _semClient;

    std::string _semHostName;

    std::string _semClientName;

    bool _isActiveGame = true;

};

#endif //LAB_2_GOAT_H
