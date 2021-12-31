#ifndef LABS_SET_H
#define LABS_SET_H

template<typename T>
class Set {
public:
    virtual bool add(T item) = 0;
    virtual bool remove(T item) = 0;
    virtual bool contains(T item) = 0;
    virtual bool empty() = 0;

    virtual ~Set() = default;
};

#endif //LABS_SET_H