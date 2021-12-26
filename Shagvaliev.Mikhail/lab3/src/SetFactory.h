//
// Created by Mikhail.Shagvaliev on 12/26/21.
//

#ifndef LAB3_SETFACTORY_H
#define LAB3_SETFACTORY_H

#include "Set.h"
#include "LazySet.h"
#include "TypedSetWrapper.h"

typedef std::function<pSet(OrderedHasher)> SetCreator;

template<typename T>
using pTypedSet = std::shared_ptr<TypedSetWrapper<T>>;

class SetFactory {
public:
    enum class SetType {
        LAZY
    };

    static pSet createSet(SetType setType, const OrderedHasher& orderedHasher) {
        switch (setType) {
            case SetType::LAZY:
                return LazySet::create(orderedHasher);
            default:
                return nullptr;
        }
    }

    template<typename T>
    static pTypedSet<T> createTypedSet(SetType setType) {
        auto set = createSet(setType, [](Any item){ return std::hash<T>{}(std::any_cast<T>(item)); });
        return (set != nullptr) ? pTypedSet<T>(new TypedSetWrapper<T>(set)) : nullptr;
    }

    template<typename T>
    static pTypedSet<T> createTypedSetWithCustomHasher(SetType setType, std::function<int(T)> orderedHasher) {
        auto set = createSet(setType, [orderedHasher](Any item){ return orderedHasher(std::any_cast<T>(item)); });
        return (set != nullptr) ? pTypedSet<T>(new TypedSetWrapper<T>(set)) : nullptr;
    }
};

#endif //LAB3_SETFACTORY_H
