#ifndef LAB_2_MESSAGE_H
#define LAB_2_MESSAGE_H

enum class GOAT_STATUS
{
    ALIVE,
    DEAD
};

struct Message {
    GOAT_STATUS status;
    int number;
};

#endif //LAB_2_MESSAGE_H
