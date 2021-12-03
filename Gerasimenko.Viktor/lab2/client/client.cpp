#include "goat.h"

int main (int argc, char* argv[]) {
    if (argc != 2) {
        syslog(LOG_ERR, "Must be argument - hostPid");
        return -1;
    }

    uint32_t hostPid = atoi(argv[1]);

    syslog(LOG_INFO, "Client is working with host (pid): %d", hostPid);

    Goat& goat = Goat::getInstance(hostPid);

    if(goat.openConnection()) {
        syslog(LOG_INFO, "Start goat");
        goat.run();
    }

    return 0;
}