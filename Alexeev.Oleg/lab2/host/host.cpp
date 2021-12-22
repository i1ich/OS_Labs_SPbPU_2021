#include <iostream>
#include "wolf.h"

int main(){
    Wolf* wolf = Wolf::getInstance();

    if(wolf->init()){
        wolf->run();
    }
    else{
        std::cout << "ERROR: can't initialize wolf" << std::endl;
    }

    syslog(LOG_INFO, "INFO: game over");
    delete wolf;

    return 0;
}
