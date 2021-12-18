#ifndef LAB3_LAZYSET_H
#define LAB3_LAZYSET_H

#include "set.h"
#include <pthread.h>
#include <climits>
#include <functional>

class LazySet : public Set {

public:
    LazySet();

    bool add(int item);

    bool remove(int item);

    bool contains(int item);

    bool empty();

    virtual ~LazySet();

private:

    class Node {

    public:
        int item;

        int hash;

        Node* next;

        pthread_mutex_t locker{};

        bool isMarked = false;

        explicit Node(int _item, int _hash) : item(_item), hash(_hash), next(nullptr) { locker = PTHREAD_MUTEX_INITIALIZER; };

        explicit Node(int val) : item(val), hash((int)std::hash<int>{}(val)), next(nullptr) { locker = PTHREAD_MUTEX_INITIALIZER; };

        void lock() { pthread_mutex_lock(&locker); }

        void unlock() { pthread_mutex_unlock(&locker); }

        ~Node() { pthread_mutex_destroy(&locker); }
    };

    Node* head;

    bool isValid(Node* pred, Node* curr) { return !pred->isMarked && !curr->isMarked && pred->next == curr; }
};

#endif //LAB3_LAZYSET_H
