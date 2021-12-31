#include "test.h"

int main(int argc, char* argv[]) {

    if (argc < 9) {
        std::cout << "8 args required: \n\t1-writers number, \n\t2-writing per writer number(equal in each writer), "
                     "\n\t3-readers number, \n\t4-reading per reader number(equal in each reader),"
                     "\n\t5-writers number for general test, \n\t6-writing per writer number for general test,"
                     "\n\t7-readers number for general test, \n\t8-reading per reader number for general test"<< std::endl;
        return 1;
    }

    int writersNum = std::stoi(argv[1]);  // writers
    int writingNum = std::stoi(argv[2]);  // writing per writer
    int readersNum = std::stoi(argv[3]);  // readers
    int readingNum = std::stoi(argv[4]);  // reading per reader

    int writersNumGeneral = std::stoi(argv[5]);  // writers general
    int writingNumGeneral = std::stoi(argv[6]);  // writing per writer general
    int readersNumGeneral = std::stoi(argv[7]);  // readers general
    int readingNumGeneral = std::stoi(argv[8]);  // reading per reader general

    // [LAZY SET TESTING]-[WRITERS]
    RunWritersTest(SetType::LAZY, TestType::SEQUENTIAL, writingNum, writersNum);
    RunWritersTest(SetType::LAZY, TestType::RANDOM, writingNum, writersNum);
    // [LAZY SET TESTING]-[READERS]
    RunReadersTest(SetType::LAZY, TestType::SEQUENTIAL, readingNum, readersNum);
    RunReadersTest(SetType::LAZY, TestType::RANDOM, readingNum, readersNum);
    // [LAZY SET TESTING]-[GENERAL]
    RunGeneralTest(SetType::LAZY, TestType::SEQUENTIAL, readingNumGeneral, readersNumGeneral, writingNumGeneral, writersNumGeneral);
    RunGeneralTest(SetType::LAZY, TestType::RANDOM, readingNumGeneral, readersNumGeneral, writingNumGeneral, writersNumGeneral);

    return 0;
}