#include <iostream>
#include "../headers/test.h"

int main(int argc, char* argv[]) {
    if(argc != 5){
        std::cout << "ERROR: invalid number arguments" << std::endl;
        return 1;
    }

    int numReaders = std::atoi(argv[1]);
    int numReading = std::atoi(argv[2]);
    int numWriters = std::atoi(argv[3]);
    int numWriting = std::atoi(argv[4]);

    try{
        std::cout << "Type data: sequential" << std::endl;
        Test::runTests(Test::FINE, Test::SEQUENTIAL, numReaders, numReading, numWriters, numWriting);
        std::cout << "Type data: random" << std::endl;
        Test::runTests(Test::FINE, Test::RANDOM, numReaders, numReading, numWriters, numWriting);
    }
    catch (...){
        std::cout << "ERROR: can't run tests" << std::endl;
        return 1;
    }
}
