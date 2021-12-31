#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>

#include "conn_seg.h"

connection * connection::CreateConnection(pid_t clientPid, bool isHost)
{
    seg *se = new seg(clientPid, isHost);

    return se;
}

seg::seg(pid_t clientPid, bool isCreator)
{
    if (isCreator)
        shmid = shmget(clientPid, BULK_SIZE, IPC_CREAT | 0660);
    else
        shmid = shmget(clientPid, BULK_SIZE, 0);
}

void seg::Open(size_t hostPid, bool isCreator)
{
    ptr = shmat(shmid, nullptr, 0);
}

void seg::Get(void* buf, size_t count)
{
    if (count > BULK_SIZE)
        throw("Too much");
    memcpy(buf, ptr, count);
}

void seg::Send(void* buf, size_t count)
{
    if (count > BULK_SIZE)
        throw("Too much");
    memcpy(ptr, buf, count);
}

void seg::Close(void)
{
}

seg::~seg(void)
{
}

