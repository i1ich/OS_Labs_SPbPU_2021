#pragma once
#include <cstddef>

class SetBase
{
public:
    virtual ~SetBase() = default;

    virtual bool add(int value) = 0;
    virtual bool remove(int value) = 0;
    virtual bool contains(int value) = 0;
    virtual size_t size() const = 0;
};