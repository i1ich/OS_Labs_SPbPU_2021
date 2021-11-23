#include <string>
#include <sys/syslog.h>
#include <iostream>
#include "Goat.h"


int main(int argc, char* argv[]){
    openlog("Goat_lab2", LOG_PID | LOG_NDELAY, LOG_USER);

    if (argc != 2) {
        syslog(LOG_ERR, "ERR: 2 arguments expected");
        std::cout << "Wrong count args" << std::endl;
        closelog();
        return EXIT_FAILURE;
    }

    int pid;
    try {
        pid = std::stoi(argv[1]);
    }
    catch (std::exception &e) {
        syslog(LOG_ERR, "ERR: Invalid pid format");
        closelog();
        return EXIT_FAILURE;
    }

    Goat& client = Goat::getInstance(pid);
    try {
        client.openConnection();
        client.start();

    } catch (std::runtime_error &error) {
        syslog(LOG_ERR, "ERR: %s", error.what());
        if (client.isWork())
            client.terminate();
        closelog();
        return EXIT_FAILURE;
    } catch (...) {
        syslog(LOG_ERR, "Undefined exception");
        if (client.isWork())
            client.terminate();
        closelog();
        return EXIT_FAILURE;
    }

    syslog(LOG_INFO, "Goat end");
    if (client.isWork())
        client.terminate();
    closelog();
    return EXIT_SUCCESS;
}
