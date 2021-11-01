#include "daemon.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    std::string configPath;
    switch (argc)
    {
    case 1:
        configPath = "config.txt";
        break;
    case 2:
        configPath = argv[1];
        break;
    default:
        std::cout << "Invalid amount of command line arguments" << std::endl;
        break;
    }

    Daemon &daemon = Daemon::create(configPath);
    daemon.start();
    return 0;
}