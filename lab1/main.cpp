#include <iostream>
#include "daemon.h"

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Incorrect command args" << std::endl;
        return -1;
    }
    return !daemon::RunDaemon(argv[1]);
}