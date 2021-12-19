#pragma once 

#include <vector>
#include "../include/set.hpp"
#include "adapter.hpp"

namespace lab
{
    template <typename ITEM_T>
    class conteiner_adapter<set<ITEM_T>, ITEM_T>
    {
    public:
        
        bool insert(ITEM_T const& item);

        bool remove(ITEM_T const& item);

        bool equals(std::vector<ITEM_T> const& arr) const;

    private:

        set<ITEM_T> m_set;
    };
}

template <typename ITEM_T>
bool lab::conteiner_adapter<lab::set<ITEM_T>, ITEM_T>::insert(ITEM_T const& item)
{
    return m_set.add(item);
}

template <typename ITEM_T>
bool lab::conteiner_adapter<lab::set<ITEM_T>, ITEM_T>::remove(ITEM_T const& item)
{
    return m_set.remove(item);
}

template <typename ITEM_T>
bool lab::conteiner_adapter<lab::set<ITEM_T>, ITEM_T>::equals(std::vector<ITEM_T> const& arr) const
{
    for (auto const& item : arr){
        if (!m_set.contains(item))
            return false;
    }
    return m_set.size() == arr.size();
}