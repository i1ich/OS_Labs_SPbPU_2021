#ifndef LAB2_DEFINITIONS_H
#define LAB2_DEFINITIONS_H

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

class Definitions {

public:
    static const int WAIT_TIMEOUT = 10;
    static const int RECONNECT_TIMEOUT = 10;

    static const int MAX_ABS_TEMPERATURE = 100;
    static const int HOST_RANDOM_SEED = 666;
};

#endif //LAB2_DEFINITIONS_H