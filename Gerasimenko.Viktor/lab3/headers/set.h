#ifndef LAB_3_SET_H
#define LAB_3_SET_H

#include "include.h"

class Set {
public:
    virtual bool add(const int& item) = 0;
    virtual bool remove(const int& item) = 0;
    virtual bool contains(const int& item) = 0;
    virtual bool empty() = 0;

    virtual ~Set(){};
};

#endif //LAB_3_SET_H
