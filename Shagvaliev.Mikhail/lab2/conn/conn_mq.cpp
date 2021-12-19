#include "conn_mq.h"
#include "message.h"

#include "../utils/logger.h"

Conn *Conn::getConnection() {
    static ConnMq connMq;
    return &connMq;
}

bool ConnMq::open(size_t hostPid, bool isOwner) {
    Logger& logger = Logger::getInstance();

    _isOwner = isOwner;
    _name = std::string("/message_queue" + std::to_string(hostPid));

    if (_isOwner) {
        struct mq_attr attr;
        attr.mq_maxmsg = 1;
        attr.mq_msgsize = sizeof(Message);
        attr.mq_curmsgs = 0;
        attr.mq_flags = 0;

        _desc = mq_open(_name.c_str(), O_RDWR | O_CREAT, 0666, &attr);
    }
    else {
        _desc= mq_open(_name.c_str(), O_RDWR);
    }

    if (_desc == -1) {
        logger.logError("mq_open failed");
        return false;
    }

    return true;
}

bool ConnMq::read(void* buf, size_t count) {
    Logger& logger = Logger::getInstance();
    bool answer = true;
    if (mq_receive(_desc, static_cast<char *>(buf), count, nullptr) == -1) {
        logger.logError("mq read");
        answer = false;
    }
    return answer;
}

bool ConnMq::write(void* buf, size_t count) {
    Logger& logger = Logger::getInstance();
    bool answer = true;
    if (mq_send(_desc, static_cast<char *>(buf), count, 0) == -1) {
        logger.logError("mq write");
        answer = false;
    }
    return answer;
}

bool ConnMq::close() {
    Logger& logger = Logger::getInstance();
    bool answer = true;

    if (mq_close(_desc) != 0) {
        logger.logError("mq close");
        answer = false;
    } else if (_isOwner && mq_unlink(_name.c_str()) != 0) {
        logger.logError("mq_unlink failed");
        answer = false;
    }

    return answer;
}
