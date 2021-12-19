#pragma once

/*
    adapter for containers
    needed to test containers with different interfaces with one template tester

    if you need to test a new container, you need to write a specification for it
*/
namespace lab
{
    template <typename CONT_T, typename ITEM_T>
    class conteiner_adapter
    {
    public: 

        bool insert(ITEM_T const& item);

        bool remove(ITEM_T const& item);

        bool equals(std::vector<ITEM_T> const& arr) const;
    };
}

