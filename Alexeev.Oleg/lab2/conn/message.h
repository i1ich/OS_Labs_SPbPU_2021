//
// Created by oleg on 11.12.2021.
//

#ifndef LAB2_MESSAGE_H
#define LAB2_MESSAGE_H

enum STATUS{
    ALIVE_GOAT = 0,
    DEAD_GOAT,
    ERROR
};

struct Message{
    STATUS status;
    int number;
};

#endif //LAB2_MESSAGE_H
