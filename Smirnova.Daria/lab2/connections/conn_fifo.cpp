#include "Connection.h"

#include <stdexcept>
#include <sys/syslog.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

void Connection::open(size_t hostPid, bool isCreator) {
    umask(000);
    m_isCreator = isCreator;
    m_connectionName = "/lab2_fifo" + std::to_string(hostPid);
    if (isCreator && mkfifo(m_connectionName.c_str(), 0666) == -1)
        throw std::runtime_error("mkfifo failed, error " + std::string(strerror(errno)));
    m_descriptor = ::open(m_connectionName.c_str(), O_RDWR);
    if (m_descriptor == -1)
        throw std::runtime_error("open fifo failed, error " + std::string(strerror(errno)));
}

void Connection::read(void *buf, size_t count) {
    if (buf == nullptr)
        throw std::runtime_error("nullptr buf passed into fifo reading");
    if (::read(m_descriptor, buf, count) == -1) {
        throw std::runtime_error("fifo reading error " + std::string(strerror(errno)));
    }
}

void Connection::write(void *buf, size_t count) {
    if (buf == nullptr)
        throw std::runtime_error("nullptr buf passed into fifo writing");
    if (::write(m_descriptor, buf, count) == -1) {
        throw std::runtime_error("fifo writing error " + std::string(strerror(errno)));
    }
}

void Connection::close() {
    if (::close(m_descriptor) < 0 || (m_isCreator && remove(m_connectionName.c_str()) < 0))
        throw std::runtime_error("error in closing " + std::string(strerror(errno)));
}