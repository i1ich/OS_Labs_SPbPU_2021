//
// Created by Mikhail.Shagvaliev on 12/26/21.
//

#ifndef LAB3_SET_H
#define LAB3_SET_H

#include "../util/definitions.h"

class Set {
public:
    virtual bool add(Any item) = 0;

    virtual bool remove(Any item) = 0;

    virtual bool contains(Any item) = 0;

    virtual bool isEmpty() = 0;

    virtual ~Set() = default;
};

#endif //LAB3_SET_H
