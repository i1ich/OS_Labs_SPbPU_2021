#ifndef __CONNECTION_H_
#define __CONNECTION_H_

#include <sys/types.h>

class connection {
public:
    static connection * CreateConnection(pid_t clientPid, bool isHost);
    virtual void Open(size_t hostPid, bool isCreator) = 0;
    virtual void Get(void* Buff, size_t Count) = 0;
    virtual void Send(void* Buff, size_t Count) = 0;
    virtual void Close(void) = 0;

    virtual ~connection(void) = default;
};

#endif //!__CONNECTION_H_