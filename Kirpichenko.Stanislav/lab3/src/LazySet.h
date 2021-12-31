#pragma once
#include <stdexcept>
#include <stack>
#include <pthread.h>
#include "MutexGuardLock.h"

template <typename T, class Hasher = std::hash<T>> 
class LazySet {
    struct Node {
        //T _data;
        size_t _hash;
        bool _fDeleted;
        Node* _next;
        pthread_mutex_t _mutex;
        Node(size_t hash);
        ~Node();
    };
    Hasher _hasher;
    Node* _head = nullptr;
    bool isValid(Node* prev, Node* cur);
public:
    LazySet();
    bool add(const T& item);
    bool remove(const T& item);
    bool contains(const T& item);
    ~LazySet();
};

#include "LazySet.tpp"