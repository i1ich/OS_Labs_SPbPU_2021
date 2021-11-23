#include <sys/syslog.h>
#include <iostream>
#include <unistd.h>
#include "GameHost.h"

int main(int argc, char* argv[]){
    openlog("HOST-LAB2", LOG_PID | LOG_NDELAY, LOG_USER);
    GameHost &host = GameHost::getInstance();
    try {
        host.openConnection();
        std::cout << "host pid = " << getpid() << std::endl;
        host.start();
    } catch (std::runtime_error &error) {
        syslog(LOG_ERR, "ERR: %s", error.what());
        if (host.isWork())
            host.terminate();
        closelog();
        return EXIT_FAILURE;
    } catch (...) {
        syslog(LOG_ERR, "ERR: Undefined exception");
        if (host.isWork())
            host.terminate();
        closelog();
        return EXIT_FAILURE;
    }
    host.terminate();
    closelog();
    return EXIT_SUCCESS;
}