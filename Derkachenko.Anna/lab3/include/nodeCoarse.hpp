#pragma once

class NodeCoarse
{
public:
    NodeCoarse(int value, NodeCoarse* prevNode = nullptr, NodeCoarse* nextNode = nullptr)
    {
        nodeValue = value;
        next = nextNode;
        prev = prevNode;
        if (nextNode)
            nextNode->prev = this;
        if (prevNode)
            prevNode->next = this;
    }

    int value() const { return nodeValue; }

    NodeCoarse *next, *prev;

private:
    int nodeValue;
};
