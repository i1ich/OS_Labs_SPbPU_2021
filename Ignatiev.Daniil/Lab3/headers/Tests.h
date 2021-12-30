#ifndef TESTS_H
#define TESTS_H

#include "Set.h"
#include "LazySet.h"

class Tests
{
public:
    enum SetType
    {
        LAZYSET
    };

    enum TestType
    {
        RANDOM,
        SEQUENTIAL
    };

    static void run(SetType setType, TestType testType, int editorsCount, int readerNum, int editsCount, int readingNum);

private:
    static void EditTest(SetType setType, TestType testType, int editorsCount, int editsCount);
    static void ReadTest(SetType setType, TestType testType, int ReadersNum, int readingNum);
    static void GeneralTest(SetType setType, TestType testType, int editorsCount, int editsCount, int ReadersNum, int readingNum);

    static bool checkAllItems(Set* set, std::vector<int> const& array);
    static void addToSet(Set* set, std::vector<int> const& array);
    static Set* CreateSet(SetType setType);
    static std::vector<std::vector<int>> GenerateData(TestType testType, int arraysNum, int itemsNum);
    static void reshape(std::vector<std::vector<int>>& items, int arraysNum, int itemsNum = -1);

    struct EditorArgs
    {
        Set* set;
        std::vector<int> itemsToWrite;
    };

    struct ReaderArgs
    {
        Set* set;
        std::vector<int> readedItems;
        pthread_mutex_t* mutex;
        TestType testType;
        int foundPos;
        std::vector<bool>* found;
    };

    static EditorArgs CreateEditorsArgs(std::vector<std::vector<int>> const& items, int threadIdx, Set *set);
    static ReaderArgs CreateReaderArgs(std::vector<std::vector<int>> const& items, int threadIdx, Set *set, std::vector<bool>& found, pthread_mutex_t& mutex);

    static void* asyncEdit(void* EditorArgs);
    static void* asyncRead(void* readerArgs);

    static const int _testsCount = 1000;
    static const int _minItem = 0;
    static const int _maxItem = 1000;
    static const int _operationsNum = 20;
    static const int _triesCount = 1000;
};


#endif
