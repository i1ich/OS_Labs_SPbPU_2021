#include <iostream>
#include "Tester.h"

int main(int argc, char* argv[]) {
  if (argc < 5) {
    std::cout << "4 args required: writers number, writing per writer number "
                 "(equal in each writer) and the same for readers.";
    return 1;
  }
  int writersNum = std::stoi(argv[1]);
  int writingNum = std::stoi(argv[2]);
  int readersNum = std::stoi(argv[3]);
  int readingNum = std::stoi(argv[4]);

  try {
    std::cout << std::endl << "OPTIMISTIC SEQUENTIAL:" << std::endl;
    Tester::run(Tester::SetType::OPTIMISTIC_SET_TYPE, Tester::TestType::SEQUENTIAL_TEST_TYPE,
                writersNum, readersNum, writingNum, readingNum);
    std::cout << std::endl << "OPTIMISTIC RANDOM:" << std::endl;
    Tester::run(Tester::SetType::OPTIMISTIC_SET_TYPE, Tester::TestType::RANDOM_TEST_TYPE, writersNum,
                readersNum, writingNum, readingNum);
    return 0;
  }
  catch (std::runtime_error &error) {
    std::cout << "ERROR: " << error.what() << std::endl;
    return 1;
  }
  catch (...) {
    std::cout << "ERROR: undefined error" << std::endl;
    return 1;
  }
}