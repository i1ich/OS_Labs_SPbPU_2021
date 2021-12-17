#include <iostream>
#include <unistd.h>

#include "wolf.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Invalid arguments. Must be two args, second arg - max clients num" << std::endl;
        return -1;
    }

    int goats_num = std::stoi(argv[1]);

    if (goats_num <= 0) {
        std::cout << "Goats num should be positive integer" << std::endl;
        return -1;
    }

    std::cout << getpid() << std::endl;

    Wolf& wolf = Wolf::instanse();
    wolf.set_max_goats_num(goats_num);
    wolf.run();

    return 0;
}