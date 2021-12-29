#include <cstdint>
#include "../headers/setFine.h"

SetFine::SetFine(){
    int value = INT32_MIN;
    _head = new Node(value, new Node(INT32_MAX, nullptr));
}

bool SetFine::add(int value) {
    _head->lock();
    Node* prev = _head;
    Node* current = _head->getNextNode();
    current->lock();

    while(current->getNextNode() != nullptr && current->getValue() < value){
        prev->unlock();
        prev = current;
        current = current->getNextNode();
        current->lock();
    }
    if(current->getValue() == value){
        prev->unlock();
        current->unlock();
        return false;
    }
    else{
        Node* node = new Node(value, current);
        if(node == nullptr){
            prev->unlock();
            current->unlock();
            return false;
        }
        prev->setNextNode(node);
        prev->unlock();
        current->unlock();
        return true;
    }
}

bool SetFine::remove(int value) {
    _head->lock();
    Node* prev = _head;
    Node* current = prev->getNextNode();
    current->lock();

    while(current->getNextNode() != nullptr && value > current->getValue()){
        prev->unlock();
        prev = current;
        current = current->getNextNode();
        current->lock();
    }

    if(current->getValue() == value){
        prev->setNextNode(current->getNextNode());
        prev->unlock();
        prev = current;
        current = current->getNextNode();
        current->lock();
        prev->unlock();
        delete prev;
        current->unlock();
        return true;
    }
    else{
        prev->unlock();
        current->unlock();
        return false;
    }
}

bool SetFine::contains(int value) {
    _head->lock();
    Node* prev = _head;
    Node* current = prev->getNextNode();
    current->lock();

    while(current->getNextNode() != nullptr && value > current->getValue()){
        prev->unlock();
        prev = current;
        current = current->getNextNode();
        current->lock();
    }

    if(current->getValue() == value){
        prev->unlock();
        current->unlock();
        return true;
    }
    else{
        prev->unlock();
        current->unlock();
        return false;
    }
}

bool SetFine::isEmpty() {
    return _head->getNextNode()->getNextNode() == nullptr;
}
