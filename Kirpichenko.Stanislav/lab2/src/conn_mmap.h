#pragma once
#include "conn_interface.h"

class Connmmap: public Conn {
    void* _shared_memory = nullptr;
    public:
    Connmmap();
    void write(const Weather& info) override;
    Weather read() override;
    ~Connmmap();
};