#include "./conn_pipe.h"

connection* connection::createConnection()
{
    return new conn_pipe();
}

bool conn_pipe::open(size_t id, bool create)
{
    if (create)
    {
        if (pipe(_desc) == -1)
        {
            syslog(LOG_ERR, "Error in pipe establishment: %s", strerror(errno));
            return false;
        }
    }
    return true;
}

bool conn_pipe::close()
{
    if (::close(_desc[0]) < 0 && ::close(_desc[1]) < 0)
    {
        return false;
    }

    return true;
};

bool conn_pipe::read(void *buf, size_t count)
{
    if (::read(_desc[0], buf, count) <= 0)
    {
        syslog(LOG_ERR, "Error in reading: %s", strerror(errno));
        return false;
    }

    return true;
}

bool conn_pipe::write(void *buf, size_t count)
{
    if (::write(_desc[1], buf, count) <= 0)
    {
        syslog(LOG_ERR, "Error in writing: %s", strerror(errno));
        return false;
    }

    return true;
}