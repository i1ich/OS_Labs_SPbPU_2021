#include "conn.h"

bool Conn::open(size_t hostPid, bool isOwner) {
    _isOwner = isOwner;
    _name = std::string("/tmp/named_pipe" + std::to_string(hostPid));

    if (_isOwner) {
        unlink(_name.c_str());
        if (mkfifo(_name.c_str(), 0666) == -1) {
            syslog(LOG_ERR, "ERROR: mkfifo failed");
            return false;
        }
    }

    _desc = ::open(_name.c_str(), O_RDWR);

    if (_desc == -1) {
        syslog(LOG_ERR, "ERROR: fifo open failed");
        return false;
    }

    return true;
}

bool Conn::read(void* buf, size_t count) {
    bool answer = true;
    if (::read(_desc, buf, count) == -1) {
        syslog(LOG_ERR, "ERROR: fifo read");
        answer = false;
    }
    return answer;
}

bool Conn::write(void* buf, size_t count) {
    bool answer = true;
    if (::write(_desc, buf, count) == -1) {
        syslog(LOG_ERR, "ERROR: fifo write");
        answer = false;
    }
    return answer;
}

bool Conn::close() {
    bool answer = true;
    if (::close(_desc) < 0 || (_isOwner && unlink(_name.c_str()) == -1)) {
        syslog(LOG_ERR, "ERROR: fifo close failed");
        answer = false;
    }
    return answer;
}


