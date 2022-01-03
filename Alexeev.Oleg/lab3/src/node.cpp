#include <iostream>
#include "../headers/node.h"

Node::Node(int value, Node *nextNode): _nextNode(nextNode), _value(value) {
    _thread = PTHREAD_MUTEX_INITIALIZER;
}

void Node::setNextNode(Node *node) {
    _nextNode = node;
}

Node* Node::getNextNode() {
    return _nextNode;
}

int Node::getValue() const {
    return _value;
}

void Node::lock() {
    if(pthread_mutex_lock(&this->_thread) != 0){
        std::cout << "ERROR: can't lock mutex" << std::endl;
    }
}

void Node::unlock() {
    pthread_mutex_unlock(&this->_thread);
}

Node::~Node(){
    pthread_mutex_destroy(&_thread);
}
