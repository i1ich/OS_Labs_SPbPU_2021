#ifndef LAB_2_CONN_H
#define LAB_2_CONN_H

#include "../utils/definitions.h"

class Conn {
public:
    static Conn* getConnection();

    virtual bool open(size_t hostPid, bool isOwner) = 0;

    virtual bool read(void* buf, size_t count) = 0;

    virtual bool write(void* buf, size_t count) = 0;

    virtual bool close() = 0;

    virtual ~Conn() = default;
};

#endif //LAB_2_CONN_H
