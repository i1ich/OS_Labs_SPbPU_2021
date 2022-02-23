
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include "conn_mq.h"

connection * connection::CreateConnection(pid_t clientPid, bool isHost)
{
    MQ *m = new MQ(clientPid, isHost);

    return m;
}

MQ::MQ(pid_t clientPid, bool isCreator)
{
    Name = "/" + std::to_string(clientPid);
}

void MQ::Open(size_t hostPid, bool isCreator)
{
    struct mq_attr Attr;

    Attr.mq_flags = 0;
    Attr.mq_maxmsg = 10;
    Attr.mq_msgsize = BULK_SIZE;
    Attr.mq_curmsgs = 0;

    if (isCreator)
        Queue = mq_open(Name.c_str(), O_CREAT | O_RDWR, 0644, &Attr);
    else
        Queue = mq_open(Name.c_str(), O_RDWR);

    if ((mqd_t)-1 == Queue)
        throw("Error opening mqueue");
}

void MQ::Get(void* Buff, size_t Count)
{
    if (Count > BULK_SIZE)
        throw("TOO BIG!!");
    mq_receive(Queue, (char *)Buff, BULK_SIZE, nullptr);
}

void MQ::Send(void* Buff, size_t Count)
{
    if (Count > BULK_SIZE)
        throw("TTOO BIG!!");
    mq_send(Queue, (const char *)Buff, Count, 0);
}

void MQ::Close(void)
{
    mq_close(Queue);
}

MQ::~MQ(void)
{
}

