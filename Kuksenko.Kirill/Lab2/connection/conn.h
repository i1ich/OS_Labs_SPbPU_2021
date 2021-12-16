#pragma once 

#include <iostream>

class Connection {
protected:
    // std::string name;
    int desc;
    bool is_creater;

    // void* shm;

public:
    static Connection* create(size_t id, bool create, size_t msg_size = 0);

    // Connection(size_t id, bool create, size_t msg_size = 0);
    virtual ~Connection() = 0;

    virtual bool Read(void* buffer, size_t count) = 0;
    virtual bool Write(void* buffer, size_t count) = 0;

};
