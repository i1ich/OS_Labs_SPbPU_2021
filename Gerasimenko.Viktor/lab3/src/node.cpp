#include "../headers/node.h"

Node::Node(int item, size_t key, Node *next) : _item(item), _key(key), _next(next) {
    _lock = PTHREAD_MUTEX_INITIALIZER;
}

void Node::lock() {
    pthread_mutex_lock(&this->_lock);
}

void Node::unlock() {
    pthread_mutex_unlock(&this->_lock);
}

int Node::getItem() {
    return this->_item;
}

size_t Node::getKey() {
    return this->_key;
}

void Node::setNext(Node* next) {
    this->_next = next;
}

Node* Node::getNext() {
    return this->_next;
}

Node::~Node() {
    pthread_mutex_destroy(&this->_lock);
}
