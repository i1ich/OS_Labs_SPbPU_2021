//
// Created by Mikhail.Shagvaliev on 12/26/21.
//

#ifndef LAB3_TYPEDSETWRAPPER_H
#define LAB3_TYPEDSETWRAPPER_H

#include "../util/definitions.h"
#include "Set.h"

template<typename T>
class TypedSetWrapper {
public:
    TypedSetWrapper(pSet& _set): set(_set) {};

    bool add(T item) {
        return set->add(item);
    }

    bool remove(T item) {
        return set->remove(item);
    }

    bool contains(T item) {
        return set->contains(item);
    }

    bool isEmpty() {
        return set->isEmpty();
    }

private:
    pSet set;
};

#endif //LAB3_TYPEDSETWRAPPER_H
