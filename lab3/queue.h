#ifndef __QUEUE_H_
#define __QUEUE_H_

#include <limits.h>

class queue
{
public:
    static const int QUEUE_MAX_SIZE = 256;
    static const int QUEUE_EMPTY = INT_MIN;
private:
    int arr[QUEUE_MAX_SIZE] = { 0 };
    int head = 0, tail = 0;
    bool empty = true;
    bool constructed = false; // to prevent using before end of constructions
public:
    queue(void);
    bool Enqueue(int val);
    int Dequeue(void);
    bool Empty(void);


#ifdef _DEBUG
    bool ContainsUnsafe(int val);
    bool& EmptyUnsafe(void);
    int* DataUnsafe(void);
    int& HeadUnsafe(void);
    int& TailUnsafe(void);
#endif
};

#endif //!__QUEUE_H_