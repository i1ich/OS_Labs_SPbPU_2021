#ifndef LAB2_GOAT_H
#define LAB2_GOAT_H

#include "../conn/conn.h"
#include "../settings/GameSettings.h"

class Goat {
public:
    ~Goat();

    static Goat& getInstance();

    void prepareGame();

    void setHostPid(int hostPid) {_hostPid = hostPid; }

    void startGame();

private:
    Goat() {};

    Goat(Goat& w) = delete;
    Goat& operator=(const Goat& w) = delete;

    int generateValue();
    bool generateAndWriteValue();

    int isAlive() { return _goatState == GOAT_STATE::ALIVE; }

    bool semWait(sem_t* sem);
    bool semSignal(sem_t* sem);
    void connectToSem(sem_t** sem, std::string semName);

    static void onSignalReceive(int sig);

    static Goat instance;

    Conn _conn;
    sem_t* _semHost;
    sem_t* _semClient;
    int _hostPid = -1;

    GOAT_STATE _goatState = GOAT_STATE::ALIVE;
};

#endif //LAB2_GOAT_H