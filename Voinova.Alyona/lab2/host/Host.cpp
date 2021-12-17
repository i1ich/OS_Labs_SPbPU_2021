#include <syslog.h>
#include <stdexcept>
#include "Host.h"

int main() {
    openlog("HOST-LAB2", LOG_PID | LOG_NDELAY| LOG_PERROR, LOG_USER);

    Host &host = Host::getInstance();

    try {
        host.openConn();

    } catch (std::runtime_error &error) {
        syslog(LOG_ERR, "%s", error.what());
        closelog();
        return EXIT_FAILURE;
    }

    try {
        host.start();

    } catch (std::runtime_error &error) {
        syslog(LOG_ERR, "%s", error.what());
        host.terminate();
        closelog();
        return EXIT_FAILURE;

    } catch (...) {
        syslog(LOG_ERR, "undefined exception");
        host.terminate();
        closelog();
        return EXIT_FAILURE;
    }

    host.terminate();
    closelog();

    return EXIT_SUCCESS;
}