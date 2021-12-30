#pragma once
#include <iostream>
#include <pthread.h>

namespace lab
{
    template <typename T, typename COMPARATOR_T = std::less<T>>
    class set
    {
    public:

        set();

        ~set();

        set(set const&);

        set& operator==(set const&);

        bool add(T const& item);

        bool remove(T const& item);

        bool contains(T const& item) const;

        size_t size() const;

    private:

        struct list_node
        {
            list_node* next;
            T item;
            list_node* clone() const;
        };

        bool async_contains(T const& item) const;

        list_node* m_top;

        mutable pthread_mutex_t m_lock; 
    };
}

template <typename T, typename COMPARATOR_T>
lab::set<T, COMPARATOR_T>::set() : 
    m_top(nullptr),
    m_lock(PTHREAD_MUTEX_INITIALIZER)
{
}

template <typename T, typename COMPARATOR_T>
lab::set<T, COMPARATOR_T>::~set()
{
    pthread_mutex_lock(&m_lock);
    auto it = m_top;
    list_node* next_it = nullptr;

    while (it != nullptr){
        next_it = it->next;
        delete it;
        it = next_it;
    }
    m_top = nullptr;
    pthread_mutex_unlock(&m_lock);
}

template <typename T, typename COMPARATOR_T>
lab::set<T, COMPARATOR_T>::set(set const& over)
{
    this->m_lock = PTHREAD_MUTEX_INITIALIZER;
    this->m_top = (over.m_top == nullptr) ? nullptr : over.m_top->clone();
}

template <typename T, typename COMPARATOR_T>
lab::set<T, COMPARATOR_T>& lab::set<T, COMPARATOR_T>::operator==(set const& over)
{
    this->m_lock = PTHREAD_MUTEX_INITIALIZER;
    this->m_top = (over.m_top == nullptr) ? nullptr : over.m_top->clone();

    return *this;
}

template <typename T, typename COMPARATOR_T>
bool lab::set<T, COMPARATOR_T>::add(T const& item)
{
    pthread_mutex_lock(&m_lock);
    if (async_contains(item)){
        pthread_mutex_unlock(&m_lock);
        return false;
    }

    auto new_it = new list_node;
    new_it->next = m_top;
    new_it->item = item;
    m_top = new_it;
    pthread_mutex_unlock(&m_lock);

    return true;
}

template <typename T, typename COMPARATOR_T>
bool lab::set<T, COMPARATOR_T>::remove(T const& item)
{
    pthread_mutex_lock(&m_lock);
    auto it = m_top;
    list_node* prev_it = nullptr;

    while (it != nullptr){
        if (!COMPARATOR_T()(it->item, item) && !COMPARATOR_T()(item, it->item)){
            auto next_it = it->next;
            delete it;
            if (prev_it == nullptr)
                m_top = next_it;
            else
                prev_it->next = next_it;
            pthread_mutex_unlock(&m_lock);

            return true;
        }
        prev_it = it;
        it = it->next;
    }
    pthread_mutex_unlock(&m_lock);

    return false;
}

template <typename T, typename COMPARATOR_T>
bool lab::set<T, COMPARATOR_T>::contains(T const& item) const
{
    pthread_mutex_lock(&m_lock);
    bool result = async_contains(item);
    pthread_mutex_unlock(&m_lock);

    return result;
}

template <typename T, typename COMPARATOR_T>
size_t lab::set<T, COMPARATOR_T>::size() const
{
    pthread_mutex_lock(&m_lock);
    size_t count = 0;
    auto it = m_top;
    while (it != nullptr){
        count++;
        it = it->next;
    }
    pthread_mutex_unlock(&m_lock);

    return count;
}

template <typename T, typename COMPARATOR_T>
bool lab::set<T, COMPARATOR_T>::async_contains(T const& item) const
{
    auto it = m_top;
    while (it != nullptr){
        if (!COMPARATOR_T()(it->item, item) && !COMPARATOR_T()(item, it->item))
            return true;
        it = it->next;
    }
    return false;
}    

template <typename T, typename COMPARATOR_T>
typename lab::set<T, COMPARATOR_T>::list_node* lab::set<T, COMPARATOR_T>::list_node::clone() const
{
    auto new_list = new list_node;
    new_list->item = item;

    if (next != nullptr)
        new_list->next = next->clone();
    else
        new_list->next = nullptr;
    
    return new_list;
}