#ifndef LAB2_VAR5_GOAT_H
#define LAB2_VAR5_GOAT_H


#include <semaphore.h>
#include "../connections/Connection.h"
#include "../utils/Message.h"

class Goat {
public:
    static Goat& getInstance();
    Goat(const Goat& goat) = delete;
    Goat& operator=(const Goat&) = delete;

    void setHostPid(int hostPid);
    void openConnection();
    void start();
    void terminate();
    bool isWork();

    ~Goat();

private:
    Goat();

    static void signalHandler(int signal);
    int guessNum();
    void wait();

    static Goat* s_goat;
    bool m_isWork;
    bool m_isAlive;
    int m_hostPid;
    Connection* m_connection;
    sem_t* m_semaphoreClient;
    sem_t* m_semaphoreHost;

};


#endif //LAB2_VAR5_GOAT_H
