#include <iostream>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include "conn.h"

static const std::string name = "./out";

namespace {

    int open_fifo(std::string const& name_id)
    {
        int fd = ::open(name_id.c_str(), O_RDWR);
        if (fd == -1)
            syslog(LOG_ERR, "cannot oper fifo: %s", strerror(errno));
        
        return fd;
    }

    int create_fifo(std::string const& name_id)
    {
        if (mkfifo(name_id.c_str(), 6060) == -1 && errno != EEXIST){
            syslog(LOG_ERR, "cannot create fifo: %s", strerror(errno));
            return errno;
        }
        return 0;
    }
}

namespace lab{
    class fd_data
    {
    public:
        int _in_fd;
        int _out_fd;
        CONN_MODE _mode;
        std::string _in_name;
        std::string _out_name;

        fd_data();
    };
}

lab::fd_data::fd_data():
    _in_fd(0),
    _out_fd(0)
{
}

lab::connaction::connaction() :
    _fd_data(new fd_data())
{
    _fd_data->_out_fd = -1;
    _fd_data->_in_fd = -1;
}

lab::connaction::~connaction()
{
    close();
    delete _fd_data;
    _fd_data = nullptr;
}

int lab::connaction::open(size_t id, CONN_MODE mode)
{
    auto out_name = name + "_host_" + std::to_string(id);
    auto in_name = name + "_client_" + std::to_string(id);

    if (create_fifo(in_name) != 0 || create_fifo(out_name) != 0)
        return -1;

    if (mode != CONN_MODE::HOST)
        std::swap(out_name, in_name);

     _fd_data->_mode = mode;
    _fd_data->_in_name = in_name;
    _fd_data->_out_name = out_name;
    _fd_data->_in_fd = open_fifo(in_name);
    _fd_data->_out_fd = open_fifo(out_name);

    if (_fd_data->_in_fd == -1 || _fd_data->_out_fd == -1)
        return -1;

    return 0;
}

int lab::connaction::read(void* buf, size_t buf_size)
{
    int rv = 0;
    if ((rv = ::read(_fd_data->_in_fd, buf, buf_size)) == -1){
        syslog(LOG_ERR, "read error: %s", strerror(errno));
        return -1;
    }
    return 0;
}

int lab::connaction::write(void* buf, size_t buf_size)
{
    if (::write(_fd_data->_out_fd, buf, buf_size) == -1){
        syslog(LOG_ERR, "write error: %s", strerror(errno));
        return -1;
    }
    return 0;
}

int lab::connaction::close()
{
    if (_fd_data->_in_fd != -1 && _fd_data->_out_fd != -1){
        if (::close(_fd_data->_out_fd) == -1 || ::close(_fd_data->_in_fd) == -1){
            syslog(LOG_ERR, "close error: %s", strerror(errno));
            return -1;
        }
        _fd_data->_out_fd = -1;
        _fd_data->_in_fd = -1;
    }

    if (_fd_data->_mode == CONN_MODE::HOST){
        remove(_fd_data->_in_name.c_str());
        remove(_fd_data->_out_name.c_str());
    }

    return 0;
}


