#ifndef LAB_2_CONN_MQ_H
#define LAB_2_CONN_MQ_H

#include "conn.h"

class ConnMq: public Conn {
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

#endif //LAB_2_CONN_MQ_H
