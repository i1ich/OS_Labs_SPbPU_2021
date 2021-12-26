#ifndef LAB3_LAZYSET_H
#define LAB3_LAZYSET_H

#include "Set.h"

class LazySet : public Set {
public:
    static pSet create(const OrderedHasher& hasher);

    bool add(Any item) override;

    bool remove(Any item) override;

    bool contains(Any item) override;

    bool isEmpty() override;

    ~LazySet() override;

private:
    LazySet(const OrderedHasher& hasher);

    class Node {
    public:
        Any item;
        int hash;

        Node* next;

        pthread_mutex_t locker{};

        bool isMarked = false;

        explicit Node(Any _item, int _hash) : item(_item), hash(_hash), next(nullptr) { locker = PTHREAD_MUTEX_INITIALIZER; };

        void lock() { pthread_mutex_lock(&locker); }

        void unlock() { pthread_mutex_unlock(&locker); }

        ~Node() { pthread_mutex_destroy(&locker); }
    };

    Node* head;

    OrderedHasher hasher;

    bool isValid(Node* pred, Node* curr) { return !pred->isMarked && !curr->isMarked && pred->next == curr; }
};

#endif //LAB3_LAZYSET_H
