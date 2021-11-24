#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>
#include "conn_shm.h"

Connshm::Connshm() {
    int fd = shm_open(_fname, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
    if (fd < 0) {
        throw std::runtime_error("Unable to create shared memory object");
    }
    ftruncate(fd, sizeof(Weather));
    _shared_memory = mmap(0, sizeof(Weather), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (_shared_memory == MAP_FAILED) {
        throw std::runtime_error("Unable to map shared memory object to address space");
    }
}

void Connshm::write(const Weather& info) {
    memcpy(_shared_memory, &info, sizeof(Weather));
}

Weather Connshm::read() {
    return *((Weather*)_shared_memory);
}

Connshm::~Connshm() {
    shm_unlink(_fname);
}