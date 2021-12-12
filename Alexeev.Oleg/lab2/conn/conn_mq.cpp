#include "conn.h"

bool Conn::open(pid_t pid, bool isHost) {
    _isHost = isHost;
    _name = std::string(MQ_ROUTE + std::to_string(pid));
    if(isHost){
        struct mq_attr attr{};
        attr.mq_maxmsg = 1;
        attr.mq_msgsize = sizeof(Message);
        attr.mq_curmsgs = 0;
        attr.mq_flags = 0;
        _descriptor = mq_open(_name.c_str(), O_RDWR | O_CREAT, 0666, &attr);
    }
    else{
        _descriptor = mq_open(_name.c_str(), O_RDWR);
    }

    if(_descriptor == -1){
        syslog(LOG_ERR, "ERROR: can't create descriptor");
        return false;
    }

    return true;
}

bool Conn::read(void* buf, size_t size) const {
    if(mq_receive(_descriptor, static_cast<char*>(buf), size, nullptr) == -1){
        syslog(LOG_ERR, "ERROR: can't read config");
        return false;
    }
    return true;
}

bool Conn::write(void* buf, size_t size) const{
    if(mq_send(_descriptor, static_cast<char*>(buf), size, 0) == -1){
        syslog(LOG_ERR, "ERROR: can't write config");
        return false;
    }
    return true;
}

bool Conn::close() const {
    if(_isHost) {
        if (mq_close(_descriptor) != -1) {
            syslog(LOG_INFO, "INFO: connection close successfully");
        } else {
            syslog(LOG_ERR, "ERROR: can't close connection");
            return false;
        }
    }
    return true;
}
