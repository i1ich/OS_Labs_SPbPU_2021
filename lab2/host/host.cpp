#include <sys/syslog.h>
#include <iostream>

int main(int argc, char* argv[]) {
    openlog("lab2_host", LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_USER);
    GameHost& host = GameHost::getInstance();
    try {
        host.openConnection();
        std::cout << "host pid = " << getpid() << '\n';
        host.start();
    }
    catch (std::exception& error) {
        syslog(LOG_ERR, "ERR: %s", error.what());
        if (host.isWork())
            host.terminate();
        closelog();
        return 1;
    }
    host.terminate();
    closelog();
    return 0;
}