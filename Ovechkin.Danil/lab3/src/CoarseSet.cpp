#include "../include/CoarseSet.h"

CoarseSet::CoarseSet() {
    int t1 = 0;
    int t2 = 0;

    head = new Node(t1, INT_MIN);
    head->next = new Node(t2, INT_MAX);
    lock = PTHREAD_MUTEX_INITIALIZER;
}

bool CoarseSet::add(int item) {

    Node *pred, *curr;
    int hash = std::hash<int>{}(item);

    pthread_mutex_lock(&lock);
    pred = head;
    curr = pred->next;

    while (curr->hash < hash) {
        pred = curr;
        curr = curr->next;
    }
    if (hash == curr->hash) {
        pthread_mutex_unlock(&lock);
        return false;
    }
    else {
        Node* newNode = new Node(item);
        newNode->next = curr;
        pred->next = newNode;

        pthread_mutex_unlock(&lock);
        return true;
    }
}

bool CoarseSet::remove(int item) {

    Node *pred, *curr;
    int hash = std::hash<int>{}(item);

    pthread_mutex_lock(&lock);
    pred = head;
    curr = pred->next;

    while (curr->hash < hash) {
        pred = curr;
        curr = curr->next;
    }

    if (hash == curr->hash) {
        pred->next = curr->next;
        pthread_mutex_unlock(&lock);

        return true;
    }
    else {
      pthread_mutex_unlock(&lock);
        return false;
    }
}

bool CoarseSet::contains(int item) {

    Node* curr;
    int hash = std::hash<int>{}(item);

    pthread_mutex_lock(&lock);
    curr = head->next;

    while (curr->hash < hash) {
        curr = curr->next;
    }

    pthread_mutex_unlock(&lock);

    return hash == curr->hash;
}

bool CoarseSet::empty() {
    return head->next->next == nullptr;
}

CoarseSet::~CoarseSet() {

    Node* curr = head;
    Node* next = head->next;
    while (next != nullptr) {
        Node* to_del = curr;
        curr = next;
        next = next->next;
        delete to_del;
    }
    delete curr;

    pthread_mutex_destroy(&lock);
}
