#ifndef LAB1_VAR16_COARSEGRAINEDSET_H
#define LAB1_VAR16_COARSEGRAINEDSET_H

#include <stdexcept>
#include <cstring>
#include "AsyncLinkedSet.h"

template<typename T, typename Compare = std::less<T>>
class CoarseGrainedSet : public AsyncLinkedSet<T, Compare> {

    using base = AsyncLinkedSet<T, Compare>;
    typedef typename base::Node Node;

public:
    CoarseGrainedSet() {
        if (pthread_mutex_init(&m_mutex, nullptr) != 0) {
            throw std::runtime_error("error in mutex initializing" + std::string(strerror(errno)));
        }
    }

    ~CoarseGrainedSet() {
        base::clean(m_head);
        pthread_mutex_destroy(&m_mutex);
    }

    virtual bool isEmpty() {
        pthread_mutex_lock(&m_mutex);
        bool empty = false;
        if (!m_head)
            empty = true;
        pthread_mutex_unlock(&m_mutex);
        return empty;
    }

    virtual bool add(T const& item) {
        pthread_mutex_lock(&m_mutex);
        if (!m_head) {
            m_head = new Node(item);
            return true;
        }
        Node* prev = nullptr;
        Node* cur = m_head;
        while(cur) {
            bool isItemHigher = Compare()(cur->value, item);
            bool isItemLover = Compare()(item, cur->value);
            if (isItemHigher) {
                prev = cur;
                cur = cur->next;
            }
            else if (isItemLover) {
                break;
            }
            else {
                pthread_mutex_unlock(&m_mutex);
                return false;
            }
        }
        Node* tmp = prev->next;
        prev->next = new Node(item);
        prev->next->next = tmp;
        pthread_mutex_unlock(&m_mutex);
        return true;
    }

    virtual bool remove(T const& item) {
        pthread_mutex_lock(&m_mutex);
        bool isFound;
        auto node = base::findPrev(m_head, item, isFound);
        if (isFound) {
            Node* toRemove;
            if (!node) {
                toRemove = m_head;
                m_head = m_head->next;
            }
            else {
                toRemove = node->next;
                node->next = node->next->next;
            }
            delete(toRemove);
        }
        pthread_mutex_unlock(&m_mutex);
        return isFound;
    }

    virtual bool contain(T const& item) {
        pthread_mutex_lock(&m_mutex);
        bool isFound;
        base::findPrev(m_head, item, isFound);
        pthread_mutex_unlock(&m_mutex);
        return isFound;
    }
private:
    pthread_mutex_t m_mutex;
    Node* m_head;
};

#endif //LAB1_VAR16_COARSEGRAINEDSET_H
