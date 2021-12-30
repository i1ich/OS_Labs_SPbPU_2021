#ifndef LAB3_LAZYSET_H
#define LAB3_LAZYSET_H

#include "Set.h"
#include <pthread.h>
#include <climits>
#include <functional>

template <typename T>
class LazySet : public Set<T> {

public:
    LazySet();

    bool add(T item);
    bool remove(T item);
    bool contains(T item);
    bool empty();

    virtual ~LazySet();

private:

    class Node {
    public:
        T item;

        int hash;

        Node* next;

        pthread_mutex_t locker{};

        bool isMarked = false;

        explicit Node(T _item, int _hash) : item(_item), hash(_hash), next(nullptr) { locker = PTHREAD_MUTEX_INITIALIZER; };
        explicit Node(T val) : item(val), hash((int)std::hash<T>{}(val)), next(nullptr) { locker = PTHREAD_MUTEX_INITIALIZER; };

        void lock() { pthread_mutex_lock(&locker); }
        void unlock() { pthread_mutex_unlock(&locker); }

        ~Node() { pthread_mutex_destroy(&locker); }
    };

    Node* head;

    bool isValid(Node* pred, Node* curr) { return !pred->isMarked && !curr->isMarked && pred->next == curr; }
};

#endif //LAB3_LAZYSET_H
