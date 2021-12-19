#include "./conn_shm.h"

connection* connection::createConnection()
{
    return new conn_shm();
}

bool conn_shm::open(size_t id, bool create)
{
    _isCreated = create;
    _connName = std::string("/shm")+ std::to_string(id);
    int mode = 0666;
    int fd;

    if (_isCreated)
    {
        fd = shm_open(_connName.c_str(), O_CREAT | O_RDWR, mode);

        if (fd == -1)
        {
            syslog(LOG_ERR, "Error in opening shared memory: %s", strerror(errno));
            return false;
        }

        ftruncate(fd, sizeof(message));
    }
    else
    {
        fd = shm_open(_connName.c_str(), O_RDWR, mode);

        if (fd == -1)
        {
            syslog(LOG_ERR, "Error in opening shared memory: %s", strerror(errno));
            return false;
        }
    }

    _sharedMemory = mmap(0, sizeof(message), PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);

    if (_sharedMemory == MAP_FAILED)
    {
        syslog(LOG_ERR, "ERROR: mmap failed, error = %s", strerror(errno));
        return false;
    }
    return true;
}

bool conn_shm::close()
{
    munmap(_sharedMemory, sizeof(message));
    shm_unlink(_connName.c_str());
    syslog(LOG_INFO, "Shm_memory connection is closed.");
    return true;
};

bool conn_shm::read(void *buf, size_t count)
{
    if (count > sizeof(message))
    {
        syslog(LOG_ERR, "Incorrect message size.");
        return false;
    }
    return (memcpy(buf, _sharedMemory, count) != nullptr);
}

bool conn_shm::write(void *buf, size_t count)
{
    if (count > sizeof(message))
    {
        syslog(LOG_ERR, "Incorrect message size.");
        return false;
    }

    return (memcpy(_sharedMemory, buf, count) != nullptr);
}