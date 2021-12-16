#include "conn.h"

#include <cstring>

#include <sys/ipc.h>
#include <sys/shm.h>

namespace {
    class ConnectionSeg : public Connection {
    private:
        void* shm;

    public:
        ConnectionSeg(size_t id, bool create, size_t msg_size);
        ~ConnectionSeg();

        bool Read(void* buffer, size_t count) override;
        bool Write(void* buffer, size_t count) override;

    };
}

Connection* Connection::create(size_t id, bool create, size_t msg_size) {
    return new ConnectionSeg(id, create, msg_size);
}

Connection::~Connection() {}


ConnectionSeg::ConnectionSeg(size_t id, bool create, size_t msg_size) {
    is_creater = create;

    int flags = create ? IPC_CREAT : 0;
    desc = shmget(id, msg_size, 0777 | flags);

    if (desc == -1) {
        std::cout << "Bad" << std::endl;
    }

    shm = shmat(desc, NULL, 0);
}

ConnectionSeg::~ConnectionSeg() {
    shmdt(shm);

    if (is_creater) {
        shmctl(desc, IPC_RMID, NULL);
    }
}

bool ConnectionSeg::Read(void* buffer, size_t count) {
    if (buffer == nullptr) {
        return false;
    }

    std::memcpy(buffer, shm, count);
    return true;
}

bool ConnectionSeg::Write(void* buffer, size_t count) {
    if (buffer == nullptr) {
        return false;
    }

    std::memcpy(shm, buffer, count);
    return true;
}
