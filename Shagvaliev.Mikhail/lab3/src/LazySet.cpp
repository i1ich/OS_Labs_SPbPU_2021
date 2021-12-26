#include "LazySet.h"

pSet LazySet::create(const OrderedHasher& _hasher) {
    return pSet(new LazySet(_hasher));
}

LazySet::LazySet(const OrderedHasher &_hasher) {
    hasher = [_hasher](Any& item) {
        try {
            return _hasher(item);
        } catch (...) {
            return INT_MIN;
        }
    };

    head = new Node(nullptr, INT_MIN);
    head->next = new Node(nullptr, INT_MAX);
}

bool LazySet::add(Any item) {
    int hash = hasher(item);

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
                Node* newNode = new Node(item, hash);

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

bool LazySet::remove(Any item) {
    int hash = hasher(item);

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

bool LazySet::contains(Any item) {
    int hash = hasher(item);
    Node* curr = head;

    while (curr->hash < hash) {
        curr = curr->next;
    }

    return curr->hash == hash && !curr->isMarked;
}

bool LazySet::isEmpty() {
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