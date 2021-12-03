#include "conn.h"

bool Conn::open(size_t hostPid, bool isOwner) {
    _isOwner = isOwner;
    _name = std::string("/tmp/socket_" + std::to_string(hostPid));
    _sockets = new int[2];

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, _name.c_str());

    if (_isOwner) {
        _sockets[0] = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (_sockets[0] == -1) {
            syslog(LOG_ERR, "ERROR: hostSocket failed");
            return false;
        }

        if (bind(_sockets[0], (struct sockaddr*) &addr, sizeof(addr)) < 0) {
            syslog(LOG_ERR, "ERROR: bind failed");
            return false;
        }

        if (listen(_sockets[0], 1) < 0 ) {
            syslog(LOG_ERR, "ERROR: listen failed");
            return false;
        }

        _sockets[1] = accept(_sockets[0], NULL, NULL);
        if (_sockets[1] < 0) {
            syslog(LOG_ERR, "ERROR: accept failed");
            return false;
        }
    } else {
        _sockets[1] = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (_sockets[1] < 0) {
            syslog(LOG_ERR, "ERROR: clientSocket failed");
            return false;
        }
        syslog(LOG_INFO, "Socket created");

        if (connect(_sockets[1],(struct sockaddr*)&addr, sizeof(addr)) < 0) {
            syslog(LOG_ERR, "ERROR: connect failed");
            return false;
        }
    }
    return true;
}

bool Conn::write(void* buf, size_t count) {
    bool answer = true;
    if(send(_sockets[1], buf, count, MSG_NOSIGNAL) < 0) {
        syslog(LOG_ERR, "ERROR: socket send failed");
        answer = false;
    }
    return answer;
}


bool Conn::read(void* buf, size_t count) {
    bool answer = true;
    if(recv(_sockets[1], buf, count, 0) < 0) {
        syslog(LOG_ERR, "ERROR: socket recieve failed");
        answer = false;
    }
    return answer;
}


bool Conn::close() {
    if (_isOwner) {
        ::close(_sockets[0]);
        ::close(_sockets[1]);
        unlink(_name.c_str());
    } else {
        ::close(_sockets[1]);
    }
    delete[] _sockets;
    return true;
}