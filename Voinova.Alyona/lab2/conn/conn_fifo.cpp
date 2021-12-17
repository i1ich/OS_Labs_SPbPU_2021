//
// Created by pikabol88 on 11/22/21.
//
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <unistd.h>
#include "IConn.h"
#include <iostream>

void IConn::openConn(size_t id, bool create) {
    _owner = create;
    _name = "/lab2_fifo";
    _id = -1;

    if (_owner && mkfifo(_name.c_str(), 0777) == -1) {
        throw std::runtime_error("mkfifo failed, error " + std::string(strerror(errno)));
    }
    _id  = open(_name.c_str(), O_RDWR);
    if (_id == -1) {
        throw std::runtime_error("mq_open failed, error " + std::string(strerror(errno)));
    }
}

void IConn::readConn(WeatherDTO *buf, size_t size) const {
    if(read(_id, reinterpret_cast<char *>(buf), size) == -1){
        throw std::runtime_error("reading error " + std::string(strerror(errno)));
    }
}

void IConn::writeConn(WeatherDTO *buf, size_t size) const {
    if(write(_id, reinterpret_cast<char *>(buf), size) == -1){
        throw std::runtime_error("writing error " + std::string(strerror(errno)));
    }
}

void IConn::closeConn() {
    if (close(_id) < 0 || (_owner && remove(_name.c_str()) < 0)) {
        throw std::runtime_error("closeConn error " + std::string(strerror(errno)));
    }
}