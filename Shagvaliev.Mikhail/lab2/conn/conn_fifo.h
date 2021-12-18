//
// Created by mikhail on 18.12.2021.
//

#include "conn.h"

#ifndef LAB_2_CONN_FIFO_H
#define LAB_2_CONN_FIFO_H

class ConnFifo: public Conn {
public:
    bool open(size_t hostPid, bool isOwner) override;

    bool read(void* buf, size_t count) override;

    bool write(void* buf, size_t count) override;

    bool close() override;

private:
    int _desc;
    bool _isOwner;
    std::string _name;
};

#endif //LAB_2_CONN_FIFO_H
