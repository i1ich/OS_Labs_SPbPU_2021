#include <iostream>
#include "../include/Parser.h"
#include "../include/Daemon.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "not enough arguments" << std::endl;
        return -1;
    }
    auto& logger = Logger::getInstance();
    logger.init();

    auto& daemon = Daemon::getInstance();
    if (daemon.setUp(argv[1]))
        daemon.run();

    logger.close();
    return 0;
}