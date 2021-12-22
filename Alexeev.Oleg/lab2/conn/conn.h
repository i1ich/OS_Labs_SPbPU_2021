//
// Created by oleg on 11.12.2021.
//

#ifndef LAB2_CONN_H
#define LAB2_CONN_H

#include "../data/common_data.h"
#include "message.h"

#define MQ_ROUTE "/mq_conn_"
#define FIFO_ROUTE "/tmp/fifo_conn_"
#define SOCKET_ROUTE "/tmp/socket_conn_"

class Conn{
public:
    static Conn* getConnection();
    virtual bool open(pid_t pid, bool isHost) = 0;
    virtual bool read(void* buf, size_t size) const = 0;
    virtual bool write(void* buf, size_t size) const = 0;
    virtual bool close() = 0;
    virtual ~Conn() = 0;
};

#endif //LAB2_CONN_H
