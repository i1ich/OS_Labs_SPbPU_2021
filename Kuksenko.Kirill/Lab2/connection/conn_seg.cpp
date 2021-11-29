#include "conn.h"

#include <cstring>

#include <sys/ipc.h>
#include <sys/shm.h>

Connection::Connection(size_t id, bool create, size_t msg_size) {
    is_creater = create;

    int flags = create ? IPC_CREAT : 0;
    fd = shmget(id, msg_size, 0777 | flags);

    if (fd == -1) {
        std::cout << "Bad" << std::endl;
    }

    shm = shmat(fd, NULL, 0);
}

Connection::~Connection() {
    shmdt(shm);

    if (is_creater) {
        shmctl(fd, IPC_RMID, NULL);
    }
}

bool Connection::Read(void* buffer, size_t count) {
    if (buffer == nullptr) {
        return false;
    }

    std::memcpy(buffer, shm, count);
    return true;
}

bool Connection::Write(void* buffer, size_t count) {
    if (buffer == nullptr) {
        return false;
    }

    std::memcpy(shm, buffer, count);
    return true;
}
