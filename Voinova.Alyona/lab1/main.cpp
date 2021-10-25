#include <iostream>
#include "Daemon.h"

int main(int argc, char **argv) {
    if(argc!=2){
        std::cout<<"Not enough arguments"<<std::endl;
        return -1;
    }

    if(Daemon::getInstance().init(argv[1])){
        Daemon::getInstance().run();
    }

    return 0;
}
