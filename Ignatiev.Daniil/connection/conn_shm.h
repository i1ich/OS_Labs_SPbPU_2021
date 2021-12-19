#ifndef CONNECTION_MQ_H
#define CONNECTION_MQ_H
#include "connection.h"
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

class conn_shm : public connection
{
private:
    void* _sharedMemory = nullptr;

public:
    bool open(size_t id, bool create) override;
    bool close() override;
    bool read(void *buf, size_t count) override;
    bool write(void *buf, size_t count) override;
};
#endif
