#include "daemon.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "not enough arguments" << std::endl;
        return -1;
    }

    if (Daemon::init(argv[1])) {
        Daemon::run();
    }

    return 0;
}
