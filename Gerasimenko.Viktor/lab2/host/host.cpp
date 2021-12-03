#include "wolf.h"

int main () {
    Wolf& wolf = Wolf::getInstance(getpid());

    if(wolf.openConnection()) {
        syslog(LOG_INFO, "Start wolf");
        wolf.run();
    }

    return 0;
}
