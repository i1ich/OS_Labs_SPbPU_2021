#include "daemon.hpp"

int main()
{
    std::string configPath = "config.txt";
    Daemon &daemon = Daemon::create(configPath);
    daemon.start();
    return 0;
}