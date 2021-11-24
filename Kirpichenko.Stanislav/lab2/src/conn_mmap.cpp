#include <unistd.h>
#include <sys/mman.h>
#include <stdexcept>
#include <string.h>
#include "conn_mmap.h"

Connmmap::Connmmap(): Conn() {
    _shared_memory = mmap(nullptr, sizeof(Weather),  PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (_shared_memory == MAP_FAILED) {
        throw std::runtime_error("Unable to get shared memory address");
    }
}

void Connmmap::write(const Weather& info) {
    memcpy(_shared_memory, &info, sizeof(Weather));
}

Weather Connmmap::read() {
    return *((Weather*)_shared_memory);
}

Connmmap::~Connmmap() {
    munmap(_shared_memory, 0);
}