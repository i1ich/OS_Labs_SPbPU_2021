#pragma once
#include "conn_interface.h"

class Connpipe: public Conn {
    int _desc[2] = {0};
    public:
    Connpipe();
    void write(const Weather& info) override;
    Weather read() override;
    ~Connpipe();
};