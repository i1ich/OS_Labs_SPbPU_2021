#include <unistd.h>
#include <sys/types.h>
#include <stdexcept>
#include "conn_pipe.h"


Connpipe::Connpipe() {
    if (pipe(_desc) < 0) {
        throw std::runtime_error("Unable to create pipe object");
    }
}

void Connpipe::write(const Weather& info) {
    if (::write(_desc[1], &info, sizeof(Weather)) < 0) {
        throw std::runtime_error("Pipe write error");
    }
}

Weather Connpipe::read() {
    Weather buf;
    if (::read(_desc[0], &buf, sizeof(Weather)) < 0) {
        throw std::runtime_error("Pipe read error");
    }
    return buf;
}

Connpipe::~Connpipe() {
    close(_desc[0]);
    close(_desc[1]);
}