//
// Created by oleg on 11.12.2021.
//

#ifndef LAB2_COMMON_DATA_H
#define LAB2_COMMON_DATA_H

#define HOST_SEMAPHORE "lab2_host_semaphore"
#define CLIENT_SEMAPHORE "lab2_client_semaphore"

#include <unistd.h>
#include <csignal>
#include <fcntl.h>
#include <sys/syslog.h>
#include <sys/stat.h>
#include <cstdlib>
#include <mqueue.h>
#include <string>
#include <semaphore.h>
#include <random>
#include <cstring>
#include <wait.h>

#endif //LAB2_COMMON_DATA_H
