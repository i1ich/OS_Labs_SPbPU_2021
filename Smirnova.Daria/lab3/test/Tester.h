#ifndef LAB3_VAR2_TESTER_H
#define LAB3_VAR2_TESTER_H


#include "../src/AsyncSet.h"

class Tester {
public:
    enum SetType {
        COARSE_SET_TYPE,
        OPTIMISTIC_SET_TYPE
    };

    enum TestType {
        RANDOM_TEST_TYPE,
        SEQUENTIAL_TEST_TYPE
    };

    static void run(SetType setType, TestType testType, int writersNum, int readerNum, int writingNum, int readingNum);

private:
    static void CheckWriters(SetType setType, TestType testType, int writersNum, int writingNum);
    static void CheckReaders(SetType setType, TestType testType, int ReadersNum, int readingNum);
    static void CheckReadersWriters(SetType setType, TestType testType, int writersNum, int writingNum, int ReadersNum,
                                    int readingNum);

    static bool checkAllItems(AsyncSet<int>* set, std::vector<int> const& array);
    static void addToSet(AsyncSet<int>* set, std::vector<int> const& array);
    static AsyncSet<int>* CreateSet(SetType setType);
    static std::vector<std::vector<int>> GenerateData(TestType testType, int arraysNum, int itemsNum);
    static void reshape(std::vector<std::vector<int>>& items, int arraysNum, int itemsNum = -1);

    struct WriterArgs {
        AsyncSet<int>* set;
        std::vector<int> itemsToWrite;
        int threadId;
    };
    struct ReaderArgs {
        AsyncSet<int>* set;
        std::vector<int> readedItems;
        int threadId;
        int foundPos;
        std::vector<bool>* found;
    };

    static WriterArgs CreateWriterArgs(std::vector<std::vector<int>> const& items, int threadIdx, AsyncSet<int> *set);
    static ReaderArgs CreateReaderArgs(std::vector<std::vector<int>> const& items, int threadIdx, AsyncSet<int> *set,
                                       std::vector<bool>& found);

    static void* asyncWrite(void* writerArgs);
    static void* asyncRead(void* readerArgs);

    static const int REPEAT_TIMES = 100;
    static const int MIN_SET_VALUE = 0;
    static const int MAX_SET_VALUE = 1000;
};


#endif //LAB3_VAR2_TESTER_H
