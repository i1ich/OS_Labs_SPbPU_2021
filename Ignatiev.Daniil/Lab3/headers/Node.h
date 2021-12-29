#ifndef NODE_H
#define NODE_H

#include <climits>
#include <pthread.h>
#include <cstddef>
#include <functional>
#include <list>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <iostream>


class Node
{
public:
    Node(int item);
    Node(int item, size_t key, Node *next);
    ~Node();
    void lock();
    void unlock();
    int getItem();
    size_t getKey();
    void setNext(Node* next);
    Node *getNext();


    int _item;
    size_t _key;
    Node *_next;
    pthread_mutex_t _lock;
    bool deleted = false;
};


#endif
