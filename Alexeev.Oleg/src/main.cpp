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
        syslog(LOG_INFO, "INFO: daemon initialize successfully");
        daemon.run();
    }
    syslog(LOG_INFO, "INFO: program end");
    return 0;
}
