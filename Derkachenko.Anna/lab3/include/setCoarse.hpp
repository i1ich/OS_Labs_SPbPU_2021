#pragma once
#include "nodeCoarse.hpp"
#include "setBase.hpp"
#include <pthread.h>

class SetCoarse : public SetBase
{
public:
    SetCoarse()
    {
        head = nullptr;
        pthread_mutex_init(&mutex, NULL);
    }

    SetCoarse(int value)
    {
        head = new NodeCoarse(value);
        tail = head;
        pthread_mutex_init(&mutex, NULL);
        ++setSize;
    }

    ~SetCoarse()
    {
        NodeCoarse* node = head;
        while (node)
        {
            NodeCoarse* next = node->next;
            delete node;
            node = next;
        }

        pthread_mutex_destroy(&mutex);
    }

    bool add(int value) override
    {
        pthread_mutex_lock(&mutex);
        bool res = addPriv(value);
        if (res)
            ++setSize;
        pthread_mutex_unlock(&mutex);
        return res;
    }

    bool remove(int value) override
    {
        pthread_mutex_lock(&mutex);
        bool res = removePriv(value);
        if (res)
            --setSize;
        pthread_mutex_unlock(&mutex);
        return res;
    }

    bool contains(int value) override
    {
        pthread_mutex_lock(&mutex);
        bool res = containsPriv(value);
        pthread_mutex_unlock(&mutex);
        return res;
    }

    size_t size() const override { return setSize; }

private:
    pthread_mutex_t mutex;
    NodeCoarse *head, *tail;
    size_t setSize = 0;

    bool addPriv(int value)
    {
        if (head == nullptr)
        {
            head = new NodeCoarse(value);
            tail = head;
            return true;
        }

        NodeCoarse* node = head;
        while (node != tail)
        {
            if (node->value() == value)
                return false;
            else
                if (node->value() < value && node->next->value() > value)
                {
                    NodeCoarse* newNode = new NodeCoarse(value, node, node->next);
                    node->next = newNode;
                return true;
                }
                else
                node = node->next;
        }

        if (node->value() == value)
            return false;
        else
        {
            node->next = new NodeCoarse(value, node);
            tail = node->next;
            return true;
        }
    }

    bool removePriv(int value)
    {
        if (head == nullptr)
            return false;

        NodeCoarse* node = head;
        while (node)
        {
            if (node->value() == value)
            {
                if (!node->prev && !node->next)
                {
                    head = nullptr;
                    tail = nullptr;
                }
                else
                    if (!node->prev)
                    {
                        head = head->next;
                        head->prev = nullptr;
                    }
                    else
                        if (!node->next)
                        {
                        tail = tail->prev;
                        tail->next = nullptr;
                        }
                        else
                        {
                            node->prev->next = node->next;
                            node->next->prev = node->prev;
                        }
                delete node;
                return true;
            }
            else
                node = node->next;
        }

        return false;
    }

    bool containsPriv(int value)
    {
        if (head == nullptr)
            return false;

        NodeCoarse* node = head;
        while (node)
        {
            if (node->value() == value)
                return true;
            else
                node = node->next;
        }

        return false;
    }
};