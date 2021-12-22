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
    void openConn(size_t id, bool create);
    void readConn(WeatherDTO *buf, size_t size) const;
    void writeConn(WeatherDTO *buf, size_t size) const;
    void closeConn();

private:
    int _id;
    bool _owner;
    std::string _name;
};

#endif //LAB2_ICONN_H
