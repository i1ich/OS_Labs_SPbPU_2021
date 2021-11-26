//
// Created by pikabol88 on 11/22/21.
//

#ifndef LAB2_ICONN_H
#define LAB2_ICONN_H
#include <cstddef>
#include <string>
#include "../data/WeatherDTO.h"

class IConn {
public:
    void open(size_t id, bool create);
    void read(WeatherDTO *buf, size_t size) const;
    void write(WeatherDTO *buf, size_t size) const;
    void close();

private:
    int _id;
    bool _owner;
    std::string _name;
};

#endif //LAB2_ICONN_H
