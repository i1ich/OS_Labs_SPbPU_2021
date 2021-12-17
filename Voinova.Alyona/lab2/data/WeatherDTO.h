//
// Created by pikabol88 on 11/22/21.
//

#ifndef LAB2_WEATHERDTO_H
#define LAB2_WEATHERDTO_H


class WeatherDTO {
private:
    unsigned int _day;
    unsigned int _month;
    unsigned int _year;
    int _temperature;

public:
    WeatherDTO(const unsigned int day = 0, const unsigned int month = 0, const unsigned int year = 0) :
            _day{day}, _month{month}, _year{year}, _temperature{0} {};

    int getTemp() const;

    void setTemp(int temperature);

    unsigned int getDay() const;

    unsigned int getMonth() const;

    unsigned int getYear() const;

};


#endif //LAB2_WEATHERDTO_H
