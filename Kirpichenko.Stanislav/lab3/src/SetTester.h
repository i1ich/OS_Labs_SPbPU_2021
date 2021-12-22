#include "Tester.h"
#include "LazySet.h"

void* writeNumsToSet(void* args);

void* readNumsFromSet(void* args);

void waitForThreads(pthread_t* threads, size_t num);

class SetWritersTester: public Tester {
    LazySet<int> _set;
    size_t _thrNum;
    size_t _testSize;
public:
    SetWritersTester(size_t threadsNum, size_t testSize);
    virtual std::string execute();
};

class SetReadersTester: public Tester {
    LazySet<int> _set;
    size_t _thrNum;
    size_t _testSize;
public:
    SetReadersTester(size_t threadsNum, size_t testSize);
    virtual std::string execute();
};

class SetCommonTester: public Tester {
    size_t _thrNum;
    size_t _testSize;
public:
    SetCommonTester(size_t threadsNum, size_t testSize);
    virtual std::string execute();
};