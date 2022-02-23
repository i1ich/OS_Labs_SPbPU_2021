#ifndef __CONN_FIFO_H_
#define __CONN_FIFO_H_

#include <string>

#include "connection.h"
// Named channels interaction
class Fifo : public connection
{
public:
    Fifo(pid_t clientPid, bool isHost);

    std::string GetNameToHost(void)
    { return FifoNameToHost; }

    std::string GetNameToClient(void)
    { return FifoNameToClient; }

    void Open(size_t hostPid, bool isCreator) override;

    void Get(void *Buff, size_t Count) override;

    void Send(void *Buff, size_t Count) override;

    void Close(void) override;

    ~Fifo(void);

private:
    std::string FifoNameToHost, FifoNameToClient;
    bool isHost;
    int fileRead, fileWrite;
};

#endif // __CONN_FIFO_H_
