#include "../utils/Message.h"
#include "conn_mq.h"

#include <mqueue.h>
#include <stdexcept>
#include <sys/syslog.h>

Connection* Connection::createConnection() {
    return new MqConnection();
}

void MqConnection::open(size_t hostPid, bool isCreator) {
    m_isCreator = isCreator;
    m_connectionName = "/lab2_mq" + std::to_string(hostPid);
    if (m_isCreator) {
        struct mq_attr attr = ((struct mq_attr) {0, 1, sizeof(Message), 0, {0}});
        m_descriptor = mq_open(m_connectionName.c_str(), O_RDWR | O_CREAT, 0666, &attr);
    } else {
        m_descriptor = mq_open(m_connectionName.c_str(), O_RDWR);
    }
    if (m_descriptor == -1)
        throw std::runtime_error("mq_open failed, error " + std::string(strerror(errno)));
    syslog(LOG_NOTICE, "Mq connection is opened");
}

void MqConnection::read(void *buf, size_t count) {
    if (buf == nullptr)
        throw std::runtime_error("nullptr buf passed into seg reading");
    if (mq_receive(m_descriptor, (char*)buf, count, nullptr) == -1)
        throw std::runtime_error("reading error " + std::string(strerror(errno)));
}

void MqConnection::write(void *buf, size_t count) {
    if (buf == nullptr)
        throw std::runtime_error("nullptr buf passed into seg reading");
    if (mq_send(m_descriptor, (char*)buf, count, 0) == -1)
        throw std::runtime_error("writing error " + std::string(strerror(errno)));
}

void MqConnection::close() {
    if (mq_close(m_descriptor)) {
        throw std::runtime_error("close error " + std::string(strerror(errno)));
    }
    if (m_isCreator && mq_unlink(m_connectionName.c_str()) != 0)
        throw std::runtime_error("close error " + std::string(strerror(errno)));
}