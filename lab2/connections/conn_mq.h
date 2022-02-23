#ifndef __CONN_MQ_H_
#define __CONN_MQ_H_

#include <string>
#include <mqueue.h>

#include "connection.h"
// Message queue
class MQ : public connection
{
public:
    MQ(pid_t clientPid, bool isCreator);
    void Open(size_t hostPid, bool isCreator) override;
    void Get(void* Buff, size_t Count) override;
    void Send(void* Buff, size_t Count) override;
    void Close(void) override;

    ~MQ(void);

private:
    static const int BULK_SIZE = 256;
    std::string Name;
    mqd_t Queue;
};

#endif // __CONN_MQ_H_
