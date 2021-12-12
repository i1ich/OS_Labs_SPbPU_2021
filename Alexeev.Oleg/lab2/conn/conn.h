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
    bool open(pid_t pid, bool isHost);
    bool read(void* buf, size_t size) const;
    bool write(void* buf, size_t size) const;
    bool close() const;

private:
    bool _isHost;
    int _descriptor;
    std::string _name;
    socklen_t _host_socket;
    socklen_t _client_socket;
};

#endif //LAB2_CONN_H
