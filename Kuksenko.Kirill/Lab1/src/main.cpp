#include "ConfigParser.h"
#include "Daemon.h"

#include <iostream>
#include <filesystem>
#include <fstream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Incorrect arguments" << std::endl;
        return 1;
    }

    Daemon* daemon = Daemon::instance(argv[1]);

    int res = daemon->run();
    Daemon::release();

    return res;
}
