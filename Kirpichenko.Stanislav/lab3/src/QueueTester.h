#include "Tester.h"
#include "SCSPQueue.h"

void* writeNumsToQueue(void* args);

void* readNumsFromQueue(void* args);

class QueueCommonTester: public Tester {
    SCSPQueue<int> _queue;
    size_t _size;
public:
    QueueCommonTester(size_t testSize);
    virtual std::string execute();
    ~QueueCommonTester() = default;
};