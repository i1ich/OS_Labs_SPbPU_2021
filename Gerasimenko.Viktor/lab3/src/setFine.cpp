#include "../headers/setFine.h"

SetFine::SetFine() {
    int t1 = 0;
    int t2 = 0;
    _head = new Node(t1, 0, new Node(t2, SIZE_MAX, nullptr));
}

bool SetFine::add(const int& item) {
    size_t key = std::hash<int>()(item);

    _head->lock();
    Node *pred = _head;
    Node *cur = pred->getNext();
    cur->lock();
    while (cur->getKey() < key) {
        pred->unlock();
        pred = cur;
        cur = cur->getNext();
        cur->lock();
    }
    if (cur->getKey() == key) {
        pred->unlock();
        cur->unlock();
        return false;
    } else {
        auto *newNode = new Node(item, key, cur);
        pred->setNext(newNode);
        pred->unlock();
        cur->unlock();
        return true;
    }
}

bool SetFine::remove(const int& item) {
    size_t key = std::hash<int>()(item);

    _head->lock();
    Node *pred = _head;
    Node *cur = pred->getNext();
    cur->lock();
    while (cur->getKey() < key) {
        pred->unlock();
        pred = cur;
        cur = cur->getNext();
        cur->lock();
    }
    if (cur->getKey() == key) {
        pred->setNext(cur->getNext());
        cur->unlock();
        delete cur;
        pred->unlock();
        return true;
    } else {
        pred->unlock();
        cur->unlock();
        return false;
    }
}

bool SetFine::contains(const int& item) {
    size_t key = std::hash<int>()(item);
    _head->lock();
    Node *pred = _head;
    Node *cur = pred->getNext();
    cur->lock();
    while (cur->getKey() < key) {
        pred->unlock();
        pred = cur;
        cur = cur->getNext();
        cur->lock();
    }
    bool isContain = cur->getKey() == key;
    pred->unlock();
    cur->unlock();
    return isContain;
}

bool SetFine::empty() {
    return _head->getNext()->getNext() == nullptr;
}

SetFine::~SetFine() {
    Node *pred = _head;
    Node *cur = pred->getNext();
    while (cur != nullptr) {
        delete pred;
        pred = cur;
        cur = cur->getNext();
    }
    delete pred;
}

