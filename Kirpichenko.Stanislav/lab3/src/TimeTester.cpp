#include <memory>
#include "TimeTester.h"
#include "SetTester.h"
#include "QueueTester.h"

constexpr size_t executionTimes = 10;

/*
    As SCSP queue is implemented, test is made only with 1 reading and 1 writing thread for that data structure.
*/

ReadersTimeTester::ReadersTimeTester(size_t testSize, size_t threadsNum, bool fRandom) {
    _fRandom = fRandom;
    _testSize = testSize;
    _thrNum = threadsNum;
}

std::string ReadersTimeTester::execute() {
    clock_t elapsedQueueTime = 0;
    clock_t elapsedSetTime = 0;
    for (size_t i = 0; i < executionTimes; i++) {
        LazySet<int> set;
        SCSPQueue<int> queue;
        std::unique_ptr<int[]> numArray(new int[_thrNum * _testSize]);
        srand((unsigned int)clock());
        for (size_t i = 0; i < _thrNum; i++) {
            for (size_t j = 0; j < _testSize; j++) {
                if (_fRandom) {
                    numArray[i * _testSize + j] = rand();    
                }
                else {
                    numArray[i * _testSize + j] = i + _thrNum;
                }
                set.add(numArray[i * _testSize + j]);
                queue.enqueue(numArray[i * _testSize + j]);
            }
        }
        clock_t startTime = clock();
        for (size_t i = 0; i < _testSize * _thrNum; i++) {
            queue.dequeue();
        }
        elapsedQueueTime += clock() - startTime;
        
        size_t argsSize = sizeof(LazySet<int>*) + sizeof(size_t) + sizeof(int*);
        std::unique_ptr<u_int8_t[]> readerArgs(new uint8_t[_thrNum * argsSize]);
        for (size_t i = 0; i < _thrNum; i++) {
            auto curPointer = readerArgs.get() + argsSize * i;
            *((LazySet<int>**)curPointer) = &set;
            *((size_t*)((u_int8_t*)curPointer + sizeof(LazySet<int>*))) = _testSize;
            *((int**)((u_int8_t*)curPointer + sizeof(LazySet<int>*) + sizeof(size_t))) = numArray.get() + i * _testSize;
        }
        int createStatus;
        std::unique_ptr<pthread_t[]> threads(new pthread_t[_testSize * _thrNum]);
        startTime = clock();
        for (size_t i = 0; i < _thrNum; i++) {
            createStatus = pthread_create(&threads[i], NULL, readNumsFromSet, readerArgs.get() + argsSize * i);
            if (createStatus != 0) {
                waitForThreads(threads.get(), i);
                throw std::runtime_error("Can't create thread in the queue test");
            }  
        }
        waitForThreads(threads.get(), _thrNum);        
        elapsedSetTime += clock() - startTime;
    }
    std::string result("SCSP queue time: ");
    double msec = (double)elapsedQueueTime / (executionTimes * CLOCKS_PER_SEC) * 1000; 
    result += std::to_string(msec) + " msec.\n";
    msec = (double)elapsedSetTime / (executionTimes * CLOCKS_PER_SEC) * 1000; 
    result += "Set with " + std::to_string(_thrNum) + " threads time: " + std::to_string(msec) + " msec.";
    return result;
}

WritersTimeTester::WritersTimeTester(size_t testSize, size_t threadsNum, bool fRandom) {
    _fRandom = fRandom;
    _testSize = testSize;
    _thrNum = threadsNum;
}

std::string WritersTimeTester::execute() {
    clock_t elapsedQueueTime = 0;
    clock_t elapsedSetTime = 0;
    for (size_t i = 0; i < executionTimes; i++) {
        LazySet<int> set;
        SCSPQueue<int> queue;
        std::unique_ptr<int[]> numArray(new int[_thrNum * _testSize]);
        srand((unsigned int)clock());
        for (size_t i = 0; i < _thrNum; i++) {
            for (size_t j = 0; j < _testSize; j++) {
                if (_fRandom) {
                    numArray[i * _testSize + j] = rand();    
                }
                else {
                    numArray[i * _testSize + j] = i + _thrNum;
                }
            }
        }
        clock_t startTime = clock();
        for (size_t i = 0; i < _testSize * _thrNum; i++) {
            queue.enqueue(numArray[i]);
        }
        elapsedQueueTime += clock() - startTime;
        
        size_t argsSize = sizeof(LazySet<int>*) + sizeof(size_t) + sizeof(int*);
        std::unique_ptr<u_int8_t[]> writerArgs(new uint8_t[_thrNum * argsSize]);
        for (size_t i = 0; i < _thrNum; i++) {
            auto curPointer = writerArgs.get() + argsSize * i;
            *((LazySet<int>**)curPointer) = &set;
            *((size_t*)((u_int8_t*)curPointer + sizeof(LazySet<int>*))) = _testSize;
            *((int**)((u_int8_t*)curPointer + sizeof(LazySet<int>*) + sizeof(size_t))) = numArray.get() + i * _testSize;
        }
        int createStatus;
        std::unique_ptr<pthread_t[]> threads(new pthread_t[_testSize * _thrNum]);
        startTime = clock();
        for (size_t i = 0; i < _thrNum; i++) {
            createStatus = pthread_create(&threads[i], NULL, writeNumsToSet, writerArgs.get() + argsSize * i);
            if (createStatus != 0) {
                waitForThreads(threads.get(), i);
                throw std::runtime_error("Can't create thread in the queue test");
            }  
        }
        waitForThreads(threads.get(), _thrNum);        
        elapsedSetTime += clock() - startTime;
    }
    std::string result("SCSP queue time: ");
    double msec = (double)elapsedQueueTime / (executionTimes * CLOCKS_PER_SEC) * 1000; 
    result += std::to_string(msec) + " msec.\n";
    msec = (double)elapsedSetTime / (executionTimes * CLOCKS_PER_SEC) * 1000; 
    result += "Set with " + std::to_string(_thrNum) + " threads time: " + std::to_string(msec) + " msec.";
    return result;
}