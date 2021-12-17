#ifndef LAB_3_SETFINE_H
#define LAB_3_SETFINE_H

#include "set.h"
#include "node.h"

class SetFine : public Set {
public:
    SetFine();

    bool add(const int& item) override;
    bool remove(const int& item) override;
    bool contains(const int& item) override;
    bool empty() override;

    ~SetFine();

private:

    Node* _head;
};

#endif //LAB_3_SETFINE_H
