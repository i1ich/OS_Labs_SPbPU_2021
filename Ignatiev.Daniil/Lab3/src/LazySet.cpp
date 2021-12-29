#include "../headers/LazySet.h"


bool LazySet::validate(Node* pred, Node* curr)
{
    if(pred->deleted || curr->deleted || pred->_next != curr)
    {
        return false;
    }

    return true;
}

LazySet::LazySet()
{
    int t1 = 0;
    int t2 = 0;
    _head = new Node(t1, 0, new Node(t2, INT_MAX, nullptr));
}

bool LazySet::add(const int& item)
{
    size_t key = std::hash<int>{}(item);
    while (true)
    {
        Node* pred = _head;
        Node* curr = _head->_next;
        while (curr->_key < key)
        {
            pred = curr;
            curr = curr->_next;
        }

        pred->lock();
        curr->lock();

        if (validate(pred, curr))
        {
            if (curr->_key == key)
            {
                curr->unlock();
                pred->unlock();
                return false;
            }
            else
            {
                Node* node = new Node(item);
                node->_next = curr;
                pred->_next = node;
                curr->unlock();
                pred->unlock();
                return true;
            }
        }
        pred->unlock();
        curr->unlock();
    }
}

bool LazySet::remove(const int& item)
{
    size_t key = std::hash<int>{}(item);
    while (true)
    {
        Node* pred = _head;
        Node* curr = _head->_next;
        while (curr->_key < key)
        {
            pred = curr;
            curr = curr->_next;
        }

        pred->lock();
        curr->lock();
        if (validate(pred, curr))
        {
            if (curr->_key != key)
            {
                curr->unlock();
                pred->unlock();
                return false;
            }
            else
            {
                curr->deleted = true;
                pred->_next = curr->_next;
                curr->unlock();
                pred->unlock();

                return true;
            }
        }

        pred->unlock();
        curr->unlock();
    }
}

bool LazySet::contains(const int& item)
{
    size_t key = std::hash<int>{}(item);
    Node *curr = _head;

    while (curr->_key < key)
    {
        curr = curr->_next;
    }

    return curr->_key == key && !curr->deleted;
}

bool LazySet::empty()
{
    return _head->getNext()->getNext() == nullptr;
}

LazySet::~LazySet()
{
    Node *pred = _head;
    Node *cur = pred->getNext();
    while (cur != nullptr)
    {
        delete pred;
        pred = cur;
        cur = cur->getNext();
    }
    delete pred;
}

