//
// Created by pikabol88 on 11/22/21.
//

#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <cstring>
#include <mqueue.h>
#include "IConn.h"

void IConn::open(size_t id, bool create) {
    _owner = create;
    _name = "lab2:queue";
    _id = -1;

    if (_owner) {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 1;
        attr.mq_curmsgs = 0;
        attr.mq_msgsize = sizeof(WeatherDTO);
        _id = mq_open(_name.c_str(), O_RDWR | O_CREAT, 0666, &attr);
    } else {
        _id = mq_open(_name.c_str(), O_RDWR);
    }
    if (_id == -1) {
        throw std::runtime_error("mq_open failed, error " + std::string(strerror(errno)));
    }
}

void IConn::read(WeatherDTO *buf, size_t size) const {
    if(mq_receive(_id, reinterpret_cast<char*>(buf), size, nullptr) == -1){
        throw std::runtime_error("reading error " + std::string(strerror(errno)));
    }
}

void IConn::write(WeatherDTO *buf, size_t size) const {
    if(mq_send(_id, reinterpret_cast<char*>(buf), size, 1) == 0){
        throw std::runtime_error("writing error " + std::string(strerror(errno)));
    }
}

void IConn::close() {
    if(mq_close(_id) == -1){
        throw std::runtime_error("writing error " + std::string(strerror(errno)));
    }
    if(_owner && mq_unlink(_name.c_str()) != 0){
        throw std::runtime_error("close error " + std::string(strerror(errno)));
    }
}