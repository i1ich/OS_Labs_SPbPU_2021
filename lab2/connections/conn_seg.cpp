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
    Seg *se = new Seg(clientPid, isHost);

    return se;
}
// Get shared memory segment
Seg::Seg(pid_t clientPid, bool isCreator)
{
    if (isCreator)
        Shmid = shmget(clientPid, BULK_SIZE, IPC_CREAT | 0660);
    else
        Shmid = shmget(clientPid, BULK_SIZE, 0);
}

void Seg::Open(size_t hostPid, bool isCreator)
{
    Ptr = shmat(Shmid, nullptr, 0);
}

void Seg::Get(void* Buff, size_t Count)
{
    if (Count > BULK_SIZE)
        throw("Too much");
    memcpy(Buff, Ptr, Count);
}

void Seg::Send(void* Buff, size_t Count)
{
    if (Count > BULK_SIZE)
        throw("Too much");
    memcpy(Ptr, Buff, Count);
}

void Seg::Close(void)
{
}

Seg::~Seg(void)
{
}

