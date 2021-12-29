//
// Created by oleg on 29.12.2021.
//

#ifndef LAB3_TEST_H
#define LAB3_TEST_H

#include "utils.h"
#include "set.h"

#define NUM_REPEATS 1000
#define NUM_ATEMPTS 20

class Test{
public:
    enum SetType{
        FINE,
    };

    enum TestType{
        RANDOM,
        SEQUENTIAL
    };

    static void runTests(SetType set, TestType test, int numReaders, int numReading, int numWriters, int numWriting);
private:
    static void testReaders(SetType set, TestType test, int numReaders, int numReading);
    static void testWriters(SetType set, TestType test, int numWriters, int numWriting);
    static void testAll(SetType set, TestType test, int numReaders, int numReading, int numWriters, int numWriting);

    static Set* getSet(SetType type);
    static std::map<int, std::vector<int>> getData(TestType type, int numProducer, int numItems);

    struct ArgsWriteTest{
        Set* set;
        std::vector<int>* writeData;
    };

    struct ArgsReadTest{
        Set* set;
        std::vector<int>* readData;
    };

    struct ArgsReadWriteTest{
        Set* set;
        std::vector<int>* data;
        std::vector<int>* check;
    };

    struct Args{
        int readers;
        int readings;
        int writers;
        int writings;
    };

    static ArgsWriteTest* createWriteArgs(std::map<int, std::vector<int>> data, int index, Set* set);
    static ArgsReadTest* createReadArgs(std::map<int, std::vector<int>> data, int index, Set* set);
    static ArgsReadWriteTest * createReadWriteArgs(std::map<int, std::vector<int>> data, int index, Set* set);

    static void* testRead(void* args);
    static void* testWrite(void* args);
    static void* testReadWrite(void* args);
    static bool checkSet(Set* set, std::vector<int>& vector);

    static void completionSet(Set* set, std::map<int, std::vector<int>> data);
    static std::vector<Args> getNumsAllTest(int numReaders, int numReadings, int numWriters, int numWritings, int readers, int readings, int writers, int writings);
    static std::vector<Args> withoutRepeat(std::vector<Args> args);
    static std::map<int, std::vector<int>> reformatData(std::map<int, std::vector<int>>, int readers, int readings);
    static bool checkReadWriteTest(std::vector<std::vector<int>> data);
};

#endif //LAB3_TEST_H
