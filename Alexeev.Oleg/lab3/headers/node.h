//
// Created by oleg on 29.12.2021.
//

#ifndef LAB3_NODE_H
#define LAB3_NODE_H

#include "utils.h"

class Node{
public:
    Node(int value, Node* nextNode);
    void setNextNode(Node* node);
    Node* getNextNode();
    [[nodiscard]] int getValue() const;
    void lock();
    void unlock();
    ~Node();
private:
    Node* _nextNode;
    int _value;
    pthread_mutex_t _thread;
};

#endif //LAB3_NODE_H
