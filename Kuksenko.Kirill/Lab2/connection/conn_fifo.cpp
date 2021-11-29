#include "conn.h"

#include <fcntl.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PATHNAME "/tmp/myfifo"

Connection::Connection(size_t id, bool create, size_t msg_size) {
    name = std::string(PATHNAME) + std::to_string(id);

    is_creater = create;

    if (create && mkfifo(name.c_str(), 0777) < 0) {
        std::cout << "Make fifo error" << std::endl;
    }


    fd = open(name.c_str(), O_RDWR | O_NONBLOCK);
    if (fd == -1) {
        std::cout << "Open error" << std::endl;
    }
}

Connection::~Connection() {
    close(fd);
    if (is_creater) {
        remove(name.c_str());
    }
}

bool Connection::Read(void* buffer, size_t count) {
    bool res = true;

    int lenght = read(fd, buffer, count);
    if (lenght == -1) {
        res = false;
    }
    
    return res;
}

bool Connection::Write(void* buffer, size_t count) {
    bool res = true;

    int length = write(fd, buffer, count);
    if (length == -1) {
        res = false;
    }

    return res;
}
