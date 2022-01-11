#ifndef LAB3_VAR2_TESTER_H
#define LAB3_VAR2_TESTER_H


#include "Set.h"
#include <vector>

class Tester {
public:
  enum SetType {
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

  static bool checkAllItems(Set<int>* set, std::vector<int> const& array);
  static void addToSet(Set<int>* set, std::vector<int> const& array);
  static Set<int>* CreateSet(SetType setType);
  static std::vector<std::vector<int>> GenerateData(TestType testType, int arraysNum, int itemsNum);
  static void reshape(std::vector<std::vector<int>>& items, int arraysNum, int itemsNum = -1);

  struct WriterArgs {
    Set<int>* set;
    std::vector<int> itemsToWrite;
  };
  struct ReaderArgs {
    Set<int>* set;
    std::vector<int> readedItems;
    pthread_mutex_t* mutex;
    TestType testType;
    int foundPos;
    std::vector<bool>* found;
  };

  static WriterArgs CreateWriterArgs(std::vector<std::vector<int>> const& items, int threadIdx, Set<int> *set);
  static ReaderArgs CreateReaderArgs(std::vector<std::vector<int>> const& items, int threadIdx, Set<int> *set,
                                     std::vector<bool>& found, pthread_mutex_t& mutex);

  static void* asyncWrite(void* writerArgs);
  static void* asyncRead(void* readerArgs);

  static const int REPEAT_TIMES = 1000;
  static const int MIN_SET_VALUE = 0;
  static const int MAX_SET_VALUE = 1000;
  static const int OPERATION_ATTEMTS_NUM = 20;
  static const int MAX_GENERATING_ATTEMPTS = 1000;
};


#endif //LAB3_VAR2_TESTER_H