#ifndef __CONN_FIFO_H_
#define __CONN_FIFO_H_

#include <string>

#include "connection.h"

class fifo : public connection
{
public:
    fifo(pid_t clientPid, bool isHost);

    std::string GetNameToHost(void)
    { return fifoNameToHost; }

    std::string GetNameToClient(void)
    { return fifoNameToClient; }

    void Open(size_t hostPid, bool isCreator) override;

    void Get(void *buf, size_t count) override;

    void Send(void *buf, size_t count) override;

    void Close(void) override;

    ~fifo(void);

private:
    std::string fifoNameToHost, fifoNameToClient;
    bool isHost;
    int fileRead, fileWrite;
};

#endif //!__CONN_FIFO_H_
