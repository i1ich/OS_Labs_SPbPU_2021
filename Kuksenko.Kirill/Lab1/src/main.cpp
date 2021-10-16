#include "ConfigParser.h"
#include "Daemon.h"
#include "DaemonSingleton.h"

#include <iostream>
#include <filesystem>
#include <fstream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Incorrect argumenst" << std::endl;
        return 1;
    }

    Daemon daemon = DaemonSingleton::instanse(argv[1])->value();
    return daemon.run();
}
