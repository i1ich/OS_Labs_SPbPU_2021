#ifndef LAB3_VAR2_ASYNCLINKEDSET_H
#define LAB3_VAR2_ASYNCLINKEDSET_H

#include "AsyncSet.h"

template<typename T, typename Compare = std::less<T>>
class AsyncLinkedSet : public AsyncSet<T, Compare> {
protected:
    struct Node {
        explicit Node(T item): value(item) {}
        T value;
        Node* next;
    };

    static void clean(Node* head) {
        Node* node = head;
        while (node) {
            Node* tmp = node->next;
            delete(node);
            node = tmp;
        }
    }

    static Node* findPrev(Node* head, T item, bool& isFound) {
        Node* prev = nullptr;
        Node* cur = head;
        isFound = false;
        while(cur) {
            if (!Compare()(cur->value, item) && !Compare()(item, cur->value)) {
                isFound = true;
                break;
            }
            prev = cur;
            cur = cur->next;
        }
        return prev;
    }
};
#endif //LAB3_VAR2_ASYNCLINKEDSET_H
