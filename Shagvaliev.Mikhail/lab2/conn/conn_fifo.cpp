#include "conn.h"

#include "../utils/logger.h"

bool Conn::open(size_t hostPid, bool isOwner) {
    Logger& logger = Logger::getInstance();

    _isOwner = isOwner;
    _name = std::string("/tmp/named_pipe" + std::to_string(hostPid));

    if (_isOwner) {
        unlink(_name.c_str());
        if (mkfifo(_name.c_str(), 0666) == -1) {
            logger.logError("mkfifo failed");
            return false;
        }
    }

    _desc = ::open(_name.c_str(), O_RDWR);

    if (_desc == -1) {
        logger.logError("fifo open failed");
        return false;
    }

    return true;
}

bool Conn::read(void* buf, size_t count) {
    Logger& logger = Logger::getInstance();
    bool answer = true;
    if (::read(_desc, buf, count) == -1) {
        logger.logError("fifo read");
        answer = false;
    }
    return answer;
}

bool Conn::write(void* buf, size_t count) {
    Logger& logger = Logger::getInstance();
    bool answer = true;
    if (::write(_desc, buf, count) == -1) {
        logger.logError("fifo write");
        answer = false;
    }
    return answer;
}

bool Conn::close() {
    Logger& logger = Logger::getInstance();
    bool answer = true;
    if (::close(_desc) < 0 || (_isOwner && unlink(_name.c_str()) == -1)) {
        logger.logError("fifo close failed");
        answer = false;
    }
    return answer;
}


