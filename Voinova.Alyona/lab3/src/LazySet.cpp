#include "../include/LazySet.h"

template class LazySet<int>;

template <typename T>
LazySet<T>::LazySet() {
   int t1 = 0;
   int t2 = 0;
   head = new Node(t1, INT_MIN);
   head->next = new Node(t2, INT_MAX);

}


template <typename T>
bool LazySet<T>::add(T item) {
    int hash = std::hash<T>{}(item);
    while (true) {
        Node* pred = head;
        Node* curr = pred->next;
        while (curr->hash < hash) {
            pred = curr;
            curr = pred->next;
        }
        pred->lock();
        curr->lock();

        if(isValid(pred, curr)) {
            if (curr->hash == hash) {
                pred->unlock();
                curr->unlock();
                return false;

            } else {
                Node* newNode = new Node(item);
                newNode->next = curr;
                pred->next = newNode;
                curr->unlock();
                pred->unlock();
                return true;
            }
        }

        curr->unlock();
        pred->unlock();
    }
}
template <typename T>
bool LazySet<T>::remove(T item) {
    int hash = std::hash<T>{}(item);
    while (true) {
        Node *pred = head;
        Node *curr = head->next;
        while (curr->hash < hash) {
            pred = curr;
            curr = curr->next;
        }
        pred->lock();
        curr->lock();
        if (isValid(pred, curr)) {
            if (curr->hash != hash) {
                curr->unlock();
                pred->unlock();

                return false;
            } else {
                curr->isMarked = true;
                pred->next = curr->next;

                curr->unlock();
                pred->unlock();
                return true;
            }
        }
        pred->unlock();
        curr->unlock();
    }
}

template <typename T>
bool LazySet<T>::contains(T item) {
    int hash = std::hash<T>{}(item);
    Node* curr = head;
    while (curr->hash < hash) {
        curr = curr->next;
    }
    return curr->hash == hash && !curr->isMarked;
}


template <typename T>
bool LazySet<T>::empty() {
    return head->next->next == nullptr;
}

template <typename T>
LazySet<T>::~LazySet() {
    Node* curr = head;
    Node* next = head->next;
    while (next != nullptr) {
        Node* tmp = curr;
        curr = next;
        next = next->next;
        delete tmp;
    }
    delete curr;
}