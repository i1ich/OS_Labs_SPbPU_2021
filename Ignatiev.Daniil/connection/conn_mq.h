#ifndef CONNECTION_MQ_H
#define CONNECTION_MQ_H
#include "connection.h"
#include <cerrno>
#include <mqueue.h>
#include <syslog.h>
#include <cstring>


class conn_mq : public connection
{
private:
    mqd_t _desc;

public:
    bool open(size_t id, bool create) override;
    bool close() override;
    bool read(void *buf, size_t count) override;
    bool write(void *buf, size_t count) override;
};
#endif
