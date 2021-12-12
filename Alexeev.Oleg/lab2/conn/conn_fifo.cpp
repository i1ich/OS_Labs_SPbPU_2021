#include "conn.h"

bool Conn::open(pid_t pid, bool isHost) {
    _name = std::string(FIFO_ROUTE + std::to_string(pid));
    if(isHost){
        unlink(_name.c_str());
        if ( mkfifo(_name.c_str(), 0666) ) {
            syslog(LOG_ERR, "ERROR: can't create fifo file");
            return false;
        }
    }
    _descriptor = ::open(_name.c_str(), O_RDWR);
    if(_descriptor == -1){
        syslog(LOG_ERR, "ERROR: can't get descriptor");
        return false;
    }
    syslog(LOG_INFO, "INFO: descriptor get successfully");
    return true;
}

bool Conn::read(void *buf, size_t size) const {
    if(::read(_descriptor, buf, size) == -1){
        syslog(LOG_ERR, "ERROR: can't read fifo file");
        return false;
    }
    return true;
}

bool Conn::write(void *buf, size_t size) const {
    if(::write(_descriptor, buf, size) == -1){
        syslog(LOG_ERR, "ERROR: can;t write in fifo file");
        return false;
    }
    return true;
}

bool Conn::close() const {
    if(::close(_descriptor) == -1){
        syslog(LOG_ERR, "ERROR: can't close fifo file");
        return false;
    }
    if(_isHost){
        if(unlink(_name.c_str()) == -1){
            syslog(LOG_ERR, "ERROR: can't unlink fifo file");
            return false;
        }
    }
    return true;
}