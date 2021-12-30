#include "setCoarse.hpp"
#include "tests.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cout << "Invalid amount of args, must be 3" << std::endl;
        return 1;
    }

    size_t writers = std::atoi(argv[1]);
    size_t readers = std::atoi(argv[2]);
    size_t maxTestNum = std::atoi(argv[3]);

    SetCoarse setCoarse;

    std::cout << "Start writers test on Coarse Set" << std::endl;
    writersTest(setCoarse, writers, maxTestNum);
    std::cout << "Writers test on Coarse Set successful" << std::endl;

    std::cout << "Start readers test on Coarse Set" << std::endl;
    readersTest(setCoarse, readers, maxTestNum);
    std::cout << "Readers test on Coarse Set successful" << std::endl;

    return 0;
}