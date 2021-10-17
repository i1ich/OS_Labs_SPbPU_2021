#include "daemon.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "not enough arguments" << std::endl;
        return -1;
    }

    Daemon &daemon = Daemon::get();

    if (daemon.init(argv[1])) {
        daemon.run();
    }

    return 0;
}