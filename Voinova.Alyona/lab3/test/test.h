#ifndef LAB3_TESTER_H
#define LAB3_TESTER_H

#include "../include/Set.h"
#include "../include/LazySet.h"

#include <iostream>
#include <vector>

enum class SetType {
    LAZY
};

enum class TestType {
    SEQUENTIAL,
    RANDOM
};

enum class Executor {
    READER,
    WRITER
};

struct Args {
    int idx;
    int step;
    int num_steps;

    std::vector<int>* items;
    std::vector<bool>* found;
    Set<int>* set;
};

void RunWritersTest(SetType setType, TestType testType, int numRecords, int numWriters);
void RunReadersTest(SetType SetType,  TestType testType,  int numRecords, int numReaders);
void RunGeneralTest(SetType setType,  TestType testType,  int readersMult, int numReaders,  int writersMult,  int numWriters);

#endif //LAB3_TESTER_H