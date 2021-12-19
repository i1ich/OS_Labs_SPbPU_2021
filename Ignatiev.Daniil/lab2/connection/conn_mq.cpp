#include "conn_mq.h"

connection* connection::createConnection()
{
    return new conn_mq();
}

bool conn_mq::open(size_t id, bool create)
{
    _isCreated = create;
    _connName = std::string("/mq") + std::to_string(id);
    int mode = 0666;

    if (_isCreated)
    {
        struct mq_attr attr = {0, 1, sizeof(message), 0, {0}};
        _desc = mq_open(_connName.c_str(), O_RDWR | O_CREAT, mode, &attr);
    }
    else
    {
        _desc = mq_open(_connName.c_str(), O_RDWR);
    }
    if (_desc == -1)
    {
        syslog(LOG_ERR, "Cannot establish connection with id = %zu, error = %s", id, strerror(errno));
        return false;
    }
    syslog(LOG_INFO, "connection with id %lu established successfully", id);
    return true;
}

bool conn_mq::close()
{
    if (!mq_close(_desc))
    {
        syslog(LOG_INFO, "connection is closed");
        return (!_isCreated || !(mq_unlink(_connName.c_str())));
    }

    syslog(LOG_ERR, "connection closing error");
    return false;
};

bool conn_mq::read(void *buf, size_t count)
{
    if (mq_receive(_desc, (char *) buf, count, nullptr) == -1)
    {
        syslog(LOG_ERR, "Problems with reading: %s", strerror(errno));
        return false;
    }
    return true;
}

bool conn_mq::write(void *buf, size_t count)
{
    if (mq_send(_desc, (char *) buf, count, 0) == -1)
    {
        syslog(LOG_ERR, "Problems with writing: %s", strerror(errno));
        return false;
    }
    return true;
}