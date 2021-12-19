#ifndef __ORACLE_H_
#define __ORACLE_H_

#include <string>
#include <csignal>

#include "../common/commondf.h"

class oracle {
public:
    oracle(const oracle&) = delete;
    oracle& operator=(const oracle&) = delete;

    weather Generate(date d);
    static void Run(pid_t hostPid);

private:
    uint32_t seed;
    bool connected;
    bool needTerm;
    static oracle *instance;

    oracle(void);
    static void signalHandler(int signum, siginfo_t* info, void *ptr);

};

#endif // !__ORACLE_H_
