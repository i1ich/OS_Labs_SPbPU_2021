#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "conn_fifo.h"

connection * connection::CreateConnection(pid_t clientPid, bool isHost)
{
    Fifo *fif = new Fifo(clientPid, isHost);
    if (isHost && (mkfifo(fif->GetNameToHost().c_str(), 0777)))// || mkfifo(fif->GetNameToClient().c_str(), 0777)))
    {
        delete fif;
        return nullptr;
    }

    return fif;
}

Fifo::Fifo(pid_t clientPid, bool isHst)
{
    isHost = isHst;
    FifoNameToHost = "/tmp/Fifo" + std::to_string(clientPid);
    FifoNameToClient = FifoNameToHost + std::to_string(clientPid);
}

void Fifo::Open(size_t hostPid, bool isCreator)
{
    if (isHost)
    {
        fileWrite = open(FifoNameToHost.c_str(), O_WRONLY);
    }
    else
    {
        fileRead = open(FifoNameToHost.c_str(), O_RDONLY);
    }

}

void Fifo::Get(void* Buff, size_t Count)
{
    if (read(fileRead, Buff, Count) < 0)
    {
        throw ("Fifo reading error");
    }
    close(fileRead);
    fileWrite = open(FifoNameToHost.c_str(), O_RDWR);
}

void Fifo::Send(void* Buff, size_t Count)
{
    if (write(fileWrite, Buff, Count) < 0)
    {
        throw ("Fifo writing error");
    }
    close(fileWrite);
    fileRead = open(FifoNameToHost.c_str(), O_RDWR);
}

void Fifo::Close(void)
{

}

Fifo::~Fifo(void)
{
}

