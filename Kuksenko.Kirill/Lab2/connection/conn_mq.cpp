#include "conn.h"

#include <cstring>
#include <fcntl.h>
#include <mqueue.h>
#include <syslog.h>

#define PATHNAME "/mq"

Connection::Connection(size_t id, bool create, size_t msg_size) {
    is_creater = create;
    name = std::string(PATHNAME) + std::to_string(id);

    int flags = O_RDWR;
    
    if (is_creater) {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 10;
        attr.mq_msgsize = msg_size;
        attr.mq_curmsgs = 0;

        flags |= O_CREAT;
        
        fd = mq_open(name.c_str(), flags, 0666, &attr);
    }
    else {
        fd = mq_open(name.c_str(), flags);
    }

    if (fd == -1) {
        syslog(LOG_ERR, "Can not open queue");
    }
}

Connection::~Connection() {
    if (mq_close(fd) == -1) {
        std::cout << "Bad close" << std::endl;
    }

    if (is_creater) {
        mq_unlink(name.c_str());
    }
}

bool Connection::Read(void* buffer, size_t count) {
    struct mq_attr attr;

    if (mq_getattr(fd, &attr) == -1) {
        syslog(LOG_ERR, "Can not get attr");
        return false;
    }

    if(mq_receive(fd, (char*)buffer, count, NULL) == -1) {
        syslog(LOG_ERR, "Can not read");
        return false;
    }

    return true;
}

bool Connection::Write(void* buffer, size_t count) {
    if (mq_send(fd, (char*)buffer, count, 0) == -1) {
        syslog(LOG_ERR, "Can not write");
        return false;
    }

    return true;
}
