//
// Created by pikabol88 on 11/26/21.
//

#include <stdexcept>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "IConn.h"

void IConn::openConn(size_t id, bool create) {
    _owner = create;
    _name = "tmp/lab2_seg";
    _id = -1;

    if (_owner) {
        _id = shmget(id, sizeof(WeatherDTO),IPC_CREAT  | 0666);
    } else {
        _id = shmget(_id, sizeof(WeatherDTO), 0666);
    }
    if (_id == -1) {
        throw std::runtime_error("shmget failed, error " + std::string(strerror(errno)));
    }
}

void IConn::writeConn(WeatherDTO *buf, size_t size) const {
    WeatherDTO *shm_buf = (WeatherDTO*) shmat(_id, nullptr, 0);
    if(shm_buf == (WeatherDTO*) -1){
        throw std::runtime_error("writting failed, error " + std::string(strerror(errno)));
    }
    memcpy(shm_buf, buf, size);
    if(shmdt(shm_buf) == -1) {
        throw std::runtime_error("writting failed, error " + std::string(strerror(errno)));
    }
}

void IConn::readConn(WeatherDTO *buf, size_t size) const {
    WeatherDTO *shm_buf = (WeatherDTO*) shmat(_id, nullptr, 0);
    memcpy(buf, shm_buf, size);
    if(shm_buf == (WeatherDTO*) -1){
        throw std::runtime_error("reading failed, error " + std::string(strerror(errno)));
    }
    if(shmdt(shm_buf) == -1) {
        throw std::runtime_error("reading failed, error " + std::string(strerror(errno)));
    }
}

void IConn::closeConn() {
    if(_owner && shmctl(_id, IPC_RMID, nullptr) < 0){
        throw std::runtime_error("clode failed, error " + std::string(strerror(errno)));
    }
}