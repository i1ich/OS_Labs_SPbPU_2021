#include <iostream>
#include "../includes/Daemon.h"

int main(int argc, char** argv)
{
    if(argc < 2){
        std::cout << "ERROR: Not config file" << std::endl;
        return 1;
    }
    Daemon daemon = Daemon::getInstance(argv[1]);
    if(daemon.init()){
        daemon.run();
    }
    return 0;
}
