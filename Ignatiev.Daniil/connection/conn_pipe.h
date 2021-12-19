#ifndef CONNECTION_MQ_H
#define CONNECTION_MQ_H
#include "connection.h"
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

class conn_pipe : public connection
{
private:
    int _desc[2] = {0};

public:
    bool open(size_t id, bool create) override;
    bool close() override;
    bool read(void *buf, size_t count) override;
    bool write(void *buf, size_t count) override;
};
#endif
