#ifndef SET_H
#define SET_H

#include <pthread.h>
#include <cstddef>
#include <functional>
#include <list>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <iostream>


class Set
{
public:
    virtual bool add(const int& item) = 0;
    virtual bool remove(const int& item) = 0;
    virtual bool contains(const int& item) = 0;
    virtual bool empty() = 0;

    virtual ~Set(){};
};

#endif
