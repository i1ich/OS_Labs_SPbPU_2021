#include <memory>
#include <atomic>
#include <vector>
#include "SetTester.h"

void* writeNumsToSet(void* args) {
    LazySet<int>* set = *((LazySet<int>**)args);
    size_t n = *((size_t*)((u_int8_t*)args + sizeof(LazySet<int>*)));
    int* numArray = *((int**)((u_int8_t*)args + sizeof(LazySet<int>*) + sizeof(size_t)));
    for (size_t i = 0; i < n; i++) {
        set->add(numArray[i]);
    }
    return nullptr;
}

void* readNumsFromSet(void* args) {
    LazySet<int>* set = *((LazySet<int>**)args);
    size_t n = *((size_t*)((u_int8_t*)args + sizeof(LazySet<int>*)));
    int* numArray = *((int**)((u_int8_t*)args + sizeof(LazySet<int>*) + sizeof(size_t)));
    for (size_t i = 0; i < n; i++) {
        set->remove(numArray[i]);
    }
    return nullptr;
}

void waitForThreads(pthread_t* threads, size_t num) {
    for (size_t i = 0; i < num; i++) {
        pthread_join(threads[i], NULL);
    }
}

SetReadersTester::SetReadersTester(size_t threadsNum, size_t testSize) {
    _thrNum = threadsNum;
    _testSize = testSize;
}

std::string SetReadersTester::execute() {
    std::unique_ptr<int[]> numArray(new int[_testSize * _thrNum]);
    srand((unsigned int)clock());
    for (size_t i = 0; i < _testSize * _thrNum; i++) {
        numArray[i] = rand();
        _set.add(numArray[i]);
    }
    std::unique_ptr<pthread_t[]> threads(new pthread_t[_testSize * _thrNum]);
    size_t argsSize = sizeof(LazySet<int>*) + sizeof(size_t) + sizeof(int*);
    std::unique_ptr<u_int8_t[]> readerArgs(new uint8_t[_thrNum * argsSize]);
    for (size_t i = 0; i < _thrNum; i++) {
        auto curPointer = readerArgs.get() + argsSize * i;
        *((LazySet<int>**)curPointer) = &_set;
        *((size_t*)((u_int8_t*)curPointer + sizeof(LazySet<int>*))) = _testSize;
        *((int**)((u_int8_t*)curPointer + sizeof(LazySet<int>*) + sizeof(size_t))) = numArray.get() + i * _testSize;
    }
    int createStatus;
    for (size_t i = 0; i < _thrNum; i++) {
        createStatus = pthread_create(&threads[i], NULL, readNumsFromSet, readerArgs.get() + argsSize * i);
        if (createStatus != 0) {
            waitForThreads(threads.get(), i);
            throw std::runtime_error("Can't create thread in the queue test");
        }  
    }
    waitForThreads(threads.get(), _thrNum);
    for (size_t i = 0; i < _testSize * _thrNum; i++) {
        if (_set.contains(numArray[i])) {
            return "FAILED";
        }
    }
    return "PASSED";
}

SetWritersTester::SetWritersTester(size_t threadsNum, size_t testSize) {
    _thrNum = threadsNum;
    _testSize = testSize;
}

std::string SetWritersTester::execute() {
    std::unique_ptr<int[]> numArray(new int[_testSize * _thrNum]);
    srand((unsigned int)clock());
    for (size_t i = 0; i < _testSize * _thrNum; i++) {
        numArray[i] = rand();
    }
    std::unique_ptr<pthread_t[]> threads(new pthread_t[_thrNum]);
    size_t argsSize = sizeof(LazySet<int>*) + sizeof(size_t) + sizeof(int*);
    std::unique_ptr<u_int8_t[]> writerArgs(new uint8_t[_thrNum * argsSize]);
    for (size_t i = 0; i < _thrNum; i++) {
        uint8_t* curPointer = writerArgs.get() + argsSize * i;
        *((LazySet<int>**)curPointer) = &_set;
        *((size_t*)((u_int8_t*)curPointer + sizeof(LazySet<int>*))) = _testSize;
        *((int**)((u_int8_t*)curPointer + sizeof(LazySet<int>*) + sizeof(size_t))) = numArray.get() + i * _testSize;
    }
    int createStatus;
    for (size_t i = 0; i < _thrNum; i++) {
        createStatus = pthread_create(&threads[i], NULL, writeNumsToSet, writerArgs.get() + argsSize * i);
        if (createStatus != 0) {
            waitForThreads(threads.get(), i);
            throw std::runtime_error("Can't create thread in the queue test");
        }  
    }
    waitForThreads(threads.get(), _thrNum);
    for (size_t i = 0; i < _testSize * _thrNum; i++) {
        if (!_set.contains(numArray[i])) {
            return "FAILED";
        }
    }
    return "PASSED";
}

SetCommonTester::SetCommonTester(size_t threadsNum, size_t testSize) {
    _thrNum = threadsNum;
    _testSize = testSize;
}

void* readSetCommonTest(void* args) {
    LazySet<int>* set = *((LazySet<int>**)args);
    size_t n = *((size_t*)((u_int8_t*)args + sizeof(LazySet<int>*)));
    size_t threadNum = *((size_t*)((u_int8_t*)args + sizeof(LazySet<int>*) + sizeof(size_t)));
    std::atomic<int>* numArray = *((std::atomic<int>**)((u_int8_t*)args + sizeof(LazySet<int>*) + 2 * sizeof(size_t)));
    for (size_t i = 0; i < n; i++) {
        int curNum = numArray[i].load();
        if (set->contains(threadNum * n + i)) {
            numArray[i].store(curNum + 1);
        }
    }
    return nullptr;
}

std::string SetCommonTester::execute() {
    size_t writeArgsSize = sizeof(LazySet<int>*) + sizeof(size_t) + sizeof(int*);
    size_t readArgsSize = sizeof(LazySet<int>*) + sizeof(size_t) + sizeof(size_t) + sizeof(std::atomic<int>*);
    for (size_t readNum = 1; readNum < _thrNum; readNum++) {
        for (size_t writeNum = 1; writeNum < _thrNum; writeNum++) {
            if (writeNum + readNum > _thrNum) {
                continue;
            }
            LazySet<int> set;
            std::unique_ptr<std::atomic<int>[]> readArray(new std::atomic<int>[_testSize * readNum]);
            std::unique_ptr<int[]> writeArray(new int[_testSize * readNum]);
            for (size_t i = 0; i < _testSize * readNum; i++) {
                writeArray[i] = i;
                readArray[i] = 0;
            }
            std::unique_ptr<u_int8_t[]> writerArgs(new uint8_t[writeNum * writeArgsSize]);
            size_t writeSize = _testSize * readNum / writeNum;
            for (size_t i = 0; i < writeNum; i++) {
                auto curPointer = writerArgs.get() + writeArgsSize * i;
                *((LazySet<int>**)curPointer) = &set;
                size_t residue = 0;
                if (i == writeNum - 1) {
                    residue = _testSize * readNum - writeSize * writeNum;
                }
                *((size_t*)((u_int8_t*)curPointer + sizeof(LazySet<int>*))) = writeSize + residue;
                *((int**)((u_int8_t*)curPointer + sizeof(LazySet<int>*) + sizeof(size_t))) = writeArray.get() + i * writeSize;
            }
            std::unique_ptr<pthread_t[]> writeThreads(new pthread_t[writeNum]);
            int createStatus;
            for (size_t i = 0; i < writeNum; i++) {
                createStatus = pthread_create(&writeThreads[i], NULL, writeNumsToSet, writerArgs.get() + writeArgsSize * i);
                if (createStatus != 0) {
                    waitForThreads(writeThreads.get(), i);
                    throw std::runtime_error("Can't create thread in the queue test");
                }
            }
            waitForThreads(writeThreads.get(), writeNum);

            std::unique_ptr<u_int8_t[]> readerArgs(new uint8_t[readNum * readArgsSize]);
            for (size_t i = 0; i < readNum; i++) {
                auto curPointer = readerArgs.get() + readArgsSize * i;
                *((LazySet<int>**)curPointer) = &set;
                *((size_t*)((u_int8_t*)curPointer + sizeof(LazySet<int>*))) = _testSize;
                *((size_t*)((u_int8_t*)curPointer + sizeof(LazySet<int>*) + sizeof(size_t))) = i;
                *((std::atomic<int>**)((u_int8_t*)curPointer + sizeof(LazySet<int>*) + 2 * sizeof(size_t))) = readArray.get() + i * _testSize;
            }
            std::unique_ptr<pthread_t[]> readThreads(new pthread_t[readNum]);
            for (size_t i = 0; i < readNum; i++) {
                createStatus = pthread_create(&readThreads[i], NULL, readSetCommonTest, readerArgs.get() + readArgsSize * i);
                if (createStatus != 0) {
                    waitForThreads(readThreads.get(), i);
                    throw std::runtime_error("Can't create thread in the queue test");
                }
            }
            waitForThreads(readThreads.get(), readNum);
            for (size_t i = 0; i < _testSize * readNum; i++) {
                if (readArray[i] != 1) {
                    return "FAILED";
                }
            }
        }
    }
    return "PASSED";
}