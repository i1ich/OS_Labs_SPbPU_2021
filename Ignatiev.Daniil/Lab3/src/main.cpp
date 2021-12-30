#include "../headers/Tests.h"

int main()
{
    int editorsCount, editsCount, readersNum, readingNum;

    std::cout << "4 args required: editors number, edits per editor number, readers number, reading per editor number"<< std::endl;
    std::cin >> editorsCount >> editsCount >> readersNum >> readingNum;

    std::cout << std::endl << "Lazy set sequential:" << std::endl;
    Tests::run(Tests::SetType::LAZYSET, Tests::TestType::SEQUENTIAL, editorsCount, readersNum, editsCount, readingNum);

    std::cout << std::endl << "Lazy set random:" << std::endl;
    Tests::run(Tests::SetType::LAZYSET, Tests::TestType::RANDOM, editorsCount, readersNum, editsCount, readingNum);
    return 0;
}
