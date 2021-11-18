#ifndef LAB_2_DEFINITIONS_H
#define LAB_2_DEFINITIONS_H

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <random>
#include <sys/stat.h>
#include <mqueue.h>
#include <iostream>
#include <syslog.h>
#include <semaphore.h>
#include <cstdlib>
#include <fcntl.h>
#include <csignal>

class GameDefinitions {

public:
    static const int WAIT_TIMEOUT = 5;
    static const int RECONNECT_TIMEOUT = 5;

    static const int MIN_WOLF_NUM = 1;
    static const int MAX_WOLF_NUM = 100;

    static const int MIN_GOAT_NUM = 1;
    static const int MAX_ALIVE_GOAT_NUM = 100;
    static const int MAX_DEAD_GOAT_NUM = 50;

    static const int ALIVE_DEVIATION = 70;
    static const int DEAD_DEVIATION = 20;

    static const int MAX_GOAT_DEATH_IN_ROW = 2;
};

#endif //LAB_2_DEFINITIONS_H
