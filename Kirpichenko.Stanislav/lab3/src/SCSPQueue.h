#pragma once
#include <stdexcept>
#include <atomic>
#include "MutexGuardLock.h"

template <typename T>
class SCSPQueue {
    struct Node {
        T _data;
        Node* _next = nullptr;
        Node(const T& item);
    };
    std::atomic<Node*> _head;
    std::atomic<Node*> _tail;
public:
    SCSPQueue();
    T dequeue();
    bool empty();
    void enqueue(const T&);
    ~SCSPQueue();
};

#include "SCSPQueue.tpp"