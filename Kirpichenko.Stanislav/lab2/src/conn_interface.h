#pragma once
#include <time.h>


struct Weather {
    inline static const char* dateFormat = "%d.%m.%Y";
    tm date;
    int temp;
};


class Conn {
    public:
    Conn() = default;
    virtual void write(const Weather& info) = 0;
    virtual Weather read() = 0;
    virtual ~Conn() = default;
};