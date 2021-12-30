#include "../include/LazySet.h"

LazySet::LazySet() {

   int t1 = 0;
   int t2 = 0;

   head = new Node(t1, INT_MIN);
   head->next = new Node(t2, INT_MAX);

}

bool LazySet::add(int item) {

    int hash = std::hash<int>{}(item);

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

bool LazySet::remove(int item) {

    int hash = std::hash<int>{}(item);

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

bool LazySet::contains(int item) {

    int hash = std::hash<int>{}(item);
    Node* curr = head;

    while (curr->hash < hash) {
        curr = curr->next;
    }

    return curr->hash == hash && !curr->isMarked;
}

bool LazySet::empty() {

    return head->next->next == nullptr;
}

LazySet::~LazySet() {

    Node* curr = head;
    Node* next = head->next;

    while (next != nullptr) {
        Node* to_del = curr;
        curr = next;
        next = next->next;
        delete to_del;
    }

    delete curr;
}