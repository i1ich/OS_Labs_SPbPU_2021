#include "../headers/tests.h"

void print_help() {
    std::cout << "4 args required: writers number, writing per writer number(equal in each writer), "
                 "readers number, reading per writer number(equal in each reader)"<< std::endl;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        print_help();
        return 1;
    }

    int writersNum = std::stoi(argv[1]);
    int writingNum = std::stoi(argv[2]);
    int readersNum = std::stoi(argv[3]);
    int readingNum = std::stoi(argv[4]);

    try {
        std::cout << std::endl << "FINE-GRAINED SEQUENTIAL:" << std::endl;
        Tests::run(Tests::SetType::SET_FINE, Tests::TestType::SEQUENTIAL_TEST_TYPE, writersNum,
                    readersNum, writingNum, readingNum);
        std::cout << std::endl << "FINE-GRAINED RANDOM:" << std::endl;
        Tests::run(Tests::SetType::SET_FINE, Tests::TestType::RANDOM_TEST_TYPE, writersNum,
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
