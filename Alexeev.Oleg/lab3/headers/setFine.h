//
// Created by oleg on 29.12.2021.
//

#ifndef LAB3_SETFINE_H
#define LAB3_SETFINE_H

#include "set.h"
#include "node.h"

class SetFine: public Set{
public:
    SetFine();

    bool add(int) override;
    bool remove(int) override;
    bool contains(int) override;
    bool isEmpty() override;
private:
    Node* _head;
};

#endif //LAB3_SETFINE_H
