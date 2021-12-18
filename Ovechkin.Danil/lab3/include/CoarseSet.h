#ifndef LABS_COARSE_SET_H
#define LABS_COARSE_SET_H

#include "set.h"
#include <pthread.h>
#include <climits>
#include <functional>


class CoarseSet : public Set {

public:
    CoarseSet();

    bool add(int item);

    bool remove(int item);

    bool contains(int item);

    bool empty();

    virtual ~CoarseSet();

private:
    class Node {
    public:
        int item;

        int hash;

        Node* next;

        explicit Node(int _item, int _hash) : item(_item), hash(_hash), next(nullptr) {};

        explicit Node(int val) : item(val), hash((int)std::hash<int>{}(val)), next(nullptr) {};
    };

    pthread_mutex_t lock{};

    Node* head{};
    
};

#endif //LABS_COARSE_SET_H
