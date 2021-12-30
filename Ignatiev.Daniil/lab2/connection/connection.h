#ifndef CONNECTION_H
#define CONNECTION_H
#include <stdlib.h>
#include <string>
#include "message.h"

class connection
{
public:
    static connection* createConnection();
    virtual bool open(size_t id, bool create) = 0;
    virtual bool close() = 0;
    virtual bool read(void *buf, size_t count) = 0;
    virtual bool write(void *buf, size_t count) = 0;

    virtual ~connection() = default;

protected:
    std::string _connName;
    bool _isCreated;
};
#endif