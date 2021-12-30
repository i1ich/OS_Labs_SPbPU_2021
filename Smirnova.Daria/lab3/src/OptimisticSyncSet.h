#ifndef LAB3_VAR2_OPTIMISTICSYNCHRONIZATIONSET_H
#define LAB3_VAR2_OPTIMISTICSYNCHRONIZATIONSET_H

#include <cstring>
#include "AsyncLinkedSet.h"

template<typename T, typename Compare = std::less<T>>
class OptimisticSyncSet : public AsyncLinkedSet<T, Compare> {

    using base = AsyncLinkedSet<T, Compare>;
    typedef typename base::Node baseNode;

public:

    OptimisticSyncSet(T const& initItem) {
        m_head = new Node(initItem);
        pthread_mutex_unlock(&m_head->mutex);
    };

    ~OptimisticSyncSet() {
        base::clean(m_head);
    }

    virtual bool isEmpty() {
        return !m_head || !m_head->next;
    }

    virtual bool add(T const& item) {
        Node* prev = nullptr;
        Node* cur = m_head;
        while(cur) {
            bool isItemHigher = Compare{}(cur->value, item);
            bool isItemLover = Compare{}(item, cur->value);
            if (isItemHigher) {
                prev = cur;
                cur = static_cast<Node*>(cur->next);
            }
            else if (isItemLover)
                break;
            else
                return false;
        }

        if (prev)
            pthread_mutex_lock(&prev->mutex);
        if (cur)
            pthread_mutex_lock(&cur->mutex);

        bool isValid = validate(prev, cur);
        if (!isValid) {
            pthread_mutex_unlock(&prev->mutex);
            if (cur)
                pthread_mutex_unlock(&cur->mutex);
            return false;
        }

        Node* tmp = cur;
        Node* newNode = new Node(item);
        prev->next = newNode;
        prev->next->next = tmp;

        pthread_mutex_unlock(&newNode->mutex);
        pthread_mutex_unlock(&prev->mutex);
        if (cur)
            pthread_mutex_unlock(&cur->mutex);
        return true;
    }

    virtual bool remove(T const& item) {
        bool isFound;
        Node* node = static_cast<Node*>(base::findPrev(m_head, item, isFound));
        if (!isFound)
            return false;
        Node* toRemove;
        if (node) {
            pthread_mutex_lock(&node->mutex);
            toRemove = static_cast<Node*>(node->next);
        }
        else {
            toRemove = m_head;
        }
        pthread_mutex_lock(&toRemove->mutex);

        bool isValid = validate(node, toRemove);
        if (isValid) {
            if (!node)
                m_head = static_cast<Node*>(m_head->next);
            else
                node->next = toRemove->next;
            delete(toRemove);
            toRemove = nullptr;
        }

        if (node) {
            pthread_mutex_unlock(&node->mutex);
        }
        if (toRemove) {
            pthread_mutex_unlock(&toRemove->mutex);
        }

        return isValid;
    }

    virtual bool contain(T const& item) {
        bool isFound;
        Node* node = static_cast<Node*>(base::findPrev(m_head, item, isFound));
        if (!isFound)
            return false;
        Node* next = nullptr;
        if (node) {
            pthread_mutex_lock(&node->mutex);
            next = static_cast<Node*>(node->next);
        }
        else {
            next = m_head;
        }
        pthread_mutex_lock(&next->mutex);

        auto isValid = validate(node, next);
        if (node) {
            pthread_mutex_unlock(&node->mutex);
        }
        pthread_mutex_unlock(&next->mutex);

        return isValid;
    }

private:
    struct Node : public baseNode {
        explicit Node(T item): baseNode(item) {
            if (pthread_mutex_init(&mutex, nullptr) != 0) {
                throw std::runtime_error("error in mutex initializing" + std::string(strerror(errno)));
            }
            pthread_mutex_lock(&mutex);
        }
        ~Node() {
            pthread_mutex_unlock(&mutex);
            pthread_mutex_destroy(&mutex);
        }
        pthread_mutex_t mutex;
    };
    
    bool validate(Node const* prevFound, Node const* found) {
        Node* cur = m_head;
        if (!cur)
            return false;
        if (!prevFound)
            return found && !Compare()(cur->value, found->value) && !Compare()(found->value, cur->value);

        while(cur->next) {
            if (!Compare()(cur->value, prevFound->value) && !Compare()(prevFound->value, cur->value)) {
                return (!cur->next && !found) || (found && !Compare()(cur->next->value, found->value) && !Compare()(found->value, cur->next->value));
            }
            cur = static_cast<Node*>(cur->next);
        }
        return !found;
    }

    Node* m_head;
};

#endif //LAB3_VAR2_OPTIMISTICSYNCHRONIZATIONSET_H
