#pragma once 

#include <iostream>

class Connection {
private:
    std::string name;
    int fd;
    bool is_creater;

    void* shm;

public:
    Connection(size_t id, bool create, size_t msg_size = 0);
    ~Connection();

    bool Read(void* buffer, size_t count);
    bool Write(void* buffer, size_t count);

};
