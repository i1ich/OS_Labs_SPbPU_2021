#pragma once
#include "conn_interface.h"

class Connshm: public Conn {
    inline static const char* _fname = "weather_shared_memory_object";
    void* _shared_memory = nullptr;
    public:
    Connshm();
    void write(const Weather& info) override;
    Weather read() override;
    ~Connshm();
};