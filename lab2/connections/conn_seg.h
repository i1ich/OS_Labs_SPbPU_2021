#ifndef __CONN_SEG_H_
#define __CONN_SEG_H_

#include <string>

#include "connection.h"
// Shared memory
class Seg : public connection
{
public:
    Seg(pid_t clientPid, bool isCreator);
    void Open(size_t hostPid, bool isCreator) override;
    void Get(void* Buff, size_t Count) override;
    void Send(void* Buff, size_t Count) override;
    void Close(void) override;

    ~Seg(void);

private:
    static const int BULK_SIZE = 256;
    int Shmid;
    void *Ptr;
};

#endif // __CONN_SEG_H_
