#ifndef LAB_3_NODE_H
#define LAB_3_NODE_H

#include "include.h"

class Node {
public:
    Node(int item, size_t key, Node *next);

    void lock();
    void unlock();
    int getItem();
    size_t getKey();
    void setNext(Node* next);
    Node *getNext();

     ~Node();

private:
    int _item;
    size_t _key;
    Node *_next;
    pthread_mutex_t _lock;
};


#endif //LAB_3_NODE_H
