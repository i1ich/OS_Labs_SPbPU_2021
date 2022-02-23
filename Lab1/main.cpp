#include <iostream>

#include "Daemon.h"

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "TYPE: ./lab1_16 filename.cfg" << std::endl;
        return -1;
    }
    return !Daemon::runDaemon(argv[1]);
}
