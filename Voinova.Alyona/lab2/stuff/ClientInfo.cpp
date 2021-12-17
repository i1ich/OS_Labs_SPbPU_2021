//
// Created by pikabol88 on 11/27/21.
//

#include "ClientInfo.h"

ClientInfo::ClientInfo(int pid) {
    _pid = pid;
    _isConnected = (pid == 0)? false: true;
}

int ClientInfo::getPid() const {
    return _pid;
}

bool ClientInfo::isConnected() const {
    return _isConnected;
}

void ClientInfo::setConnected(bool isConnected) {
    _isConnected = isConnected;
}
