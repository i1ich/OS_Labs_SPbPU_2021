#include "queue.h"

queue::queue(void)
{
    for (int i = 0; i < QUEUE_MAX_SIZE; i++)
        arr[i] = QUEUE_EMPTY;
    constructed = true;
}

bool queue::Enqueue(int val)
{
    if (!constructed)
        return false;

    int h, t;
    bool e;
    __atomic_load(&head, &h, __ATOMIC_RELAXED);
    __atomic_load(&tail, &t, __ATOMIC_RELAXED);
    __atomic_load(&empty, &e, __ATOMIC_RELAXED);

    if (h == t && !e)
        return false;

    arr[t] = val;
    e = false;
    __atomic_store(&empty, &e, __ATOMIC_RELAXED);
    t = (t + 1) % QUEUE_MAX_SIZE;
    __atomic_store(&tail, &t, __ATOMIC_RELAXED);
    return true;
}

int queue::Dequeue(void)
{
    int h, t;
    bool e;
    __atomic_load(&head, &h, __ATOMIC_RELAXED);
    __atomic_load(&tail, &t, __ATOMIC_RELAXED);
    __atomic_load(&empty, &e, __ATOMIC_RELAXED);
    if (h == t && e)
        return QUEUE_EMPTY;

    int n = arr[h];
    if (__sync_bool_compare_and_swap(&head, h, (h + 1) % QUEUE_MAX_SIZE))
    {
        __atomic_load(&head, &h, __ATOMIC_RELAXED);
        __atomic_load(&tail, &t, __ATOMIC_RELAXED);
        __atomic_load(&empty, &e, __ATOMIC_RELAXED);
        if (h == t && !e)
        {
            e = true;
            __atomic_store(&empty, &e, __ATOMIC_RELAXED);
        }
        return n;
    }
    else
        return QUEUE_EMPTY;
}

bool queue::Empty(void)
{
    int h, t;
    bool e;
    __atomic_load(&head, &h, __ATOMIC_RELAXED);
    __atomic_load(&tail, &t, __ATOMIC_RELAXED);
    __atomic_load(&empty, &e, __ATOMIC_RELAXED);
    return h == t && e;
}

#ifdef _DEBUG
bool queue::ContainsUnsafe(int val)
{
    for (int i = head; i != tail; i = (i + 1) % QUEUE_MAX_SIZE)
        if (arr[i] == val)
            return true;
    return false;
}

bool& queue::EmptyUnsafe(void)
{
    return empty;
}

int* queue::DataUnsafe(void)
{
    return arr;
}

int& queue::HeadUnsafe(void)
{
    return head;
}

int& queue::TailUnsafe(void)
{
    return tail;
}

#endif