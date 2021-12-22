#include <memory>
#include <atomic>
#include <unistd.h>
#include "QueueTester.h"

QueueCommonTester::QueueCommonTester(size_t testSize) {
    _size = testSize;
}

void* writeNumsToQueue(void* args) {
    SCSPQueue<int>* queue = *((SCSPQueue<int>**)args);
    size_t n = *((size_t*)((u_int8_t*)args + sizeof(SCSPQueue<int>*)));
    for (size_t i = 0; i < n; i++) {
        queue->enqueue(i);
    }
    return nullptr;
}

void* readNumsFromQueue(void* args) {
    SCSPQueue<int>* queue = *((SCSPQueue<int>**)args);
    size_t n = *((size_t*)((u_int8_t*)args + sizeof(SCSPQueue<int>*)));
    int* numArray = *((int**)((u_int8_t*)args + sizeof(SCSPQueue<int>*) + sizeof(size_t)));
    size_t read = 0;
    size_t yieldsNum = 0, maxYields = 10 * n;
    while (read < n && yieldsNum < maxYields) {
        if (!queue->empty()) {
            numArray[read] = queue->dequeue();
            read++;
        }
        else {
            sched_yield(); // pthread_yield is deprecated
            yieldsNum++;
        }
    }
    return nullptr;
}

/*
    As SCSP queue was implemented, the only reasonable test is when 1 thread writes to the queue and
    parallely 2nd thread reads the data. This test is made below, data is checked for the fifo order. 
*/
std::string QueueCommonTester::execute() {
    std::unique_ptr<int[]> numArray(new int[_size]);
    //auto numArray = new int[_size];
    u_int8_t writerArgs[sizeof(SCSPQueue<int>*) + sizeof(size_t)] = {0};
    *((SCSPQueue<int>**)writerArgs) = &_queue;
    *((size_t*)((u_int8_t*)writerArgs + sizeof(SCSPQueue<int>*))) = _size;
    u_int8_t readerArgs[sizeof(SCSPQueue<int>*) + sizeof(size_t) + sizeof(int*)] = {0};
    *((SCSPQueue<int>**)readerArgs) = &_queue;
    *((size_t*)((u_int8_t*)readerArgs + sizeof(SCSPQueue<int>*))) = _size;
    *((int**)((u_int8_t*)readerArgs + sizeof(SCSPQueue<int>*) + sizeof(size_t))) = numArray.get();

    pthread_t writeThread;
    pthread_t readThread;
    int createStatus;
    createStatus = pthread_create(&writeThread, NULL, writeNumsToQueue, writerArgs);
    if (createStatus != 0) {
        throw std::runtime_error("Can't create thread in the queue test");
    }
    createStatus = pthread_create(&readThread, NULL, readNumsFromQueue, readerArgs);
    if (createStatus != 0) {
        pthread_join(writeThread, NULL);
        throw std::runtime_error("Can't create thread in the queue test");
    }
    pthread_join(writeThread, NULL);
    pthread_join(readThread, NULL);
    for (int i = 0; i < (int)_size; i++) {
        if (numArray[i] != i) {
            return "FAILED";
        }
    }
    return "PASSED";
}