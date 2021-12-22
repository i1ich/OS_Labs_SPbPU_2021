#include "conn.h"
#include <sys/socket.h>
#include <sys/un.h>

namespace{
    class ConnSocket: public Conn{
    public:
        bool open(pid_t pid, bool isHost) override;
        bool read(void* buf, size_t size) const override;
        bool write(void* buf, size_t size) const override;
        bool close() override;
        ~ConnSocket() override = default;
    private:
        bool _isHost;
        std::string _name;
        socklen_t _host_socket;
        socklen_t _client_socket;
    };
}

Conn* Conn::getConnection() {
    return new ConnSocket();
}

bool ConnSocket::open(pid_t pid, bool isHost) {
    _isHost = isHost;
    _name = std::string(SOCKET_ROUTE + std::to_string(pid));

    struct sockaddr_un add;
    add.sun_family = AF_UNIX;
    std::strncpy(add.sun_path, _name.c_str(), sizeof(add.sun_path) - 1);
    if (isHost) {
        _host_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if(_host_socket < 0){
            syslog(LOG_ERR, "ERROR: can't create host socket");
            return true;
        }
        if (bind(_host_socket, (struct sockaddr *) &add, sizeof(add)) < 0) {
            syslog(LOG_ERR, "ERROR: can't bind socket and structure");
            return false;
        }
        if (listen(_host_socket, 1) < 0) {
            syslog(LOG_ERR, "ERROR: can't listen processes");
            return false;
        }
        _client_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        _client_socket = accept(_host_socket, NULL, NULL);
        if (_client_socket < 0) {
            syslog(LOG_ERR, "ERROR: can't accept client socket");
            return false;
        }
    } else {
        _client_socket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (_client_socket < 0) {
            syslog(LOG_ERR, "ERROR: can't create client socket");
            return false;
        }
        if (connect(_client_socket, (const struct sockaddr *) &add, sizeof(struct sockaddr_un)) == -1) {
            syslog(LOG_ERR, "ERROR: can't connect sockets");
            return false;
        }
    }
    return true;
}

bool ConnSocket::read(void *buf, size_t size) const {
    if (recv(_client_socket, buf, size, 0) < 0) {
        syslog(LOG_ERR, "ERROR: can't read from socket");
        return false;
    }
    return true;
}

bool ConnSocket::write(void *buf, size_t size) const {
    if (send(_client_socket, buf, size, MSG_NOSIGNAL) < 0) {
        syslog(LOG_ERR, "ERROR: can't write in socket");
        return false;
    }
    return true;
}

bool ConnSocket::close() {
    if (_isHost) {
        if (::close(_client_socket) == -1) {
            syslog(LOG_ERR, "ERROR: can't close client socket");
            return false;
        }
        if (::close(_host_socket) == -1) {
            syslog(LOG_ERR, "ERROR: can't close host socket");
            return false;
        }
        if (unlink(_name.c_str()) == -1) {
            syslog(LOG_ERR, "ERROR: can't unlink socket file");
            return false;
        }
    } else {
        if (::close(_client_socket) == -1) {
            syslog(LOG_ERR, "ERROR: can't close client socket");
            return false;
        }
    }
    return true;
}

Conn::~Conn(){}
