#ifndef LABS_SET_H
#define LABS_SET_H

class Set {
public:
    virtual bool add(int item) = 0;

    virtual bool remove(int item) = 0;

    virtual bool contains(int item) = 0;

    virtual bool empty() = 0;

    virtual ~Set() = default;
};

#endif //LABS_SET_H
