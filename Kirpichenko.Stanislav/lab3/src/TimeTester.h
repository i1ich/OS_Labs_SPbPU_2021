#include "SCSPQueue.h"
#include "LazySet.h"
#include "Tester.h"

class ReadersTimeTester: public Tester {
    SCSPQueue<int> queue;
    LazySet<int> set;
    bool _fRandom;
    size_t _testSize;
    size_t _thrNum;
public:
    ReadersTimeTester(size_t testSize, size_t threadsNum, bool fRandom);
    virtual std::string execute();
};

class WritersTimeTester: public Tester {
    SCSPQueue<int> queue;
    LazySet<int> set;
    bool _fRandom;
    size_t _testSize;
    size_t _thrNum;
public:
    WritersTimeTester(size_t testSize, size_t threadsNum, bool fRandom);
    virtual std::string execute();
};