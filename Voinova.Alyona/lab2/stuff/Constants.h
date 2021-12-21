//
// Created by pikabol88 on 11/27/21.
//

#ifndef LAB2_CONSTANTS_H
#define LAB2_CONSTANTS_H
#include <string>

class Constants {
public:
    static const std::string SEM_HOST_NAME;
    static const std::string SEM_CLIENT_NAME;

    static const int TIMEOUT = 5;
    static const int TIMEOUT_CLIENT = 60;
    static const int MIN_TEMPERATURE = -45;
    static const int MAX_TEMPERATURE = 45;

    static const std::string EXIT_CODE;

};

const std::string Constants::SEM_HOST_NAME="/HOST_LAB2";
const std::string Constants::SEM_CLIENT_NAME="/CLIENT_LAB2";
const std::string Constants::EXIT_CODE="exit";

#endif //LAB2_CONSTANTS_H
