#ifndef LAB2_VAR5_GAMERULES_H
#define LAB2_VAR5_GAMERULES_H

#include <string>

struct GameRules {
    static const std::string SEM_HOST_NAME;
    static const std::string SEM_CLIENT_NAME;
    static const int TIMEOUT = 1000000000;

    static const int MIN_WOLF_NUM = 1;
    static const int MAX_WOLF_NUM = 100;

    static const int MIN_ALIVE_GOAT_NUM = 1;
    static const int MAX_ALIVE_GOAT_NUM = 100;

    static const int MIN_DEAD_GOAT_NUM = 1;
    static const int MAX_DEAD_GOAT_NUM = 50;

    static const int ALIVE_GOAT_SPACE_MUL = 70;
    static const int DEAD_GOAT_SPACE_MUL = 20;

    static const int All_DEAD_END_TIMES = 2;
};

const std::string GameRules::SEM_HOST_NAME="HOST_LAB2";
const std::string GameRules::SEM_CLIENT_NAME="CLIENT_LAB2";

#endif //LAB2_VAR5_GAMERULES_H
