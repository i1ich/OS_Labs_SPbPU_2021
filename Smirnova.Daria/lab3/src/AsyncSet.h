#include <functional>

#ifndef LAB3_VAR2_SET_H
#define LAB3_VAR2_SET_H

template<typename T, typename Compare = std::less<T>>
class AsyncSet {
public:
    virtual bool add(T const& item) = 0;
    virtual bool remove(T const& item) = 0;
    virtual bool contain(T const& item) = 0;

    virtual bool isEmpty() = 0;

    virtual ~AsyncSet() = default;
};

#endif //LAB3_VAR2_SET_H
