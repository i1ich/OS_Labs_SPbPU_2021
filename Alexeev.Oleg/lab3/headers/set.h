//
// Created by oleg on 29.12.2021.
//

#ifndef LAB3_SET_H
#define LAB3_SET_H

class Set{
public:
    virtual bool add(int) = 0;
    virtual bool remove(int) = 0;
    virtual bool contains(int) = 0;
    virtual bool isEmpty() = 0;
};

#endif //LAB3_SET_H
