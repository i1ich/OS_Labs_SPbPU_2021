#ifndef LAZYSET_H
#define LAZYSET_H

#include "Set.h"
#include "Node.h"

class LazySet : public Set
{
public:
    LazySet();

    bool add(const int& item) override;
    bool remove(const int& item) override;
    bool contains(const int& item) override;
    bool empty() override;

    ~LazySet();

private:
    Node* _head;
    bool validate(Node* pred, Node* curr);
};

#endif
