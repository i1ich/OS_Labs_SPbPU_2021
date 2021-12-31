#ifndef __CONN_MQ_H_
#define __CONN_MQ_H_

#include <string>
#include <mqueue.h>

#include "connection.h"

class mq : public connection
{
public:
    mq(pid_t clientPid, bool isCreator);
    void Open(size_t hostPid, bool isCreator) override;
    void Get(void* buf, size_t count) override;
    void Send(void* buf, size_t count) override;
    void Close(void) override;

    ~mq(void);

private:
    static const int BULK_SIZE = 256;
    std::string name;
    mqd_t queue;
};

#endif //!__CONN_MQ_H_
