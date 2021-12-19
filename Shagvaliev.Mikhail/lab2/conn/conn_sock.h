#ifndef LAB_2_CONN_SOCK_H
#define LAB_2_CONN_SOCK_H

#include "conn.h"

class ConnSock: public Conn {
public:
    bool open(size_t hostPid, bool isOwner);

    bool read(void* buf, size_t count);

    bool write(void* buf, size_t count);

    bool close();

private:
    int _desc;
    bool _isOwner;
    std::string _name;
    int* _sockets;
};

#endif //LAB_2_CONN_SOCK_H
