//
// Created by pikabol88 on 11/22/21.
//

#include "WeatherDTO.h"

void WeatherDTO::setTemp(int temperature) {
    this->_temperature = temperature;
}

int WeatherDTO::getTemp() const {
    return _temperature;
}

unsigned int WeatherDTO::getDay() const {
    return _day;
}

unsigned int WeatherDTO::getMonth() const {
    return _month;
}

unsigned int WeatherDTO::getYear() const {
    return _year;
}