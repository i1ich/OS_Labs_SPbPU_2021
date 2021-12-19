#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "conn_fifo.h"

connection * connection::CreateConnection(pid_t clientPid, bool isHost)
{
    fifo *fif = new fifo(clientPid, isHost);
    if (isHost && (mkfifo(fif->GetNameToHost().c_str(), 0777)))// || mkfifo(fif->GetNameToClient().c_str(), 0777)))
    {
        delete fif;
        return nullptr;
    }

    return fif;
}

fifo::fifo(pid_t clientPid, bool isHst)
{
    isHost = isHst;
    fifoNameToHost = "/tmp/fifo" + std::to_string(clientPid);
    fifoNameToClient = fifoNameToHost + std::to_string(clientPid);
}

void fifo::Open(size_t hostPid, bool isCreator)
{
    if (isHost)
    {
        fileWrite = open(fifoNameToHost.c_str(), O_WRONLY);
    }
    else
    {
        fileRead = open(fifoNameToHost.c_str(), O_RDONLY);
    }
/*    if (isHost)
    {
        fileRead = open(fifoNameToHost.c_str(), O_RDONLY);
        fileWrite = open(fifoNameToClient.c_str(), O_WRONLY);
    }
    else
    {
        fileWrite = open(fifoNameToHost.c_str(), O_WRONLY);
        fileRead = open(fifoNameToClient.c_str(), O_RDONLY | O_NONBLOCK);
    }
    if (fileRead <= 0 || fileWrite <= 0)
    {
        if (fileRead > 0)
            close(fileRead);
        if (fileWrite > 0)
            close(fileWrite);
        throw ("fifo opening error");
    }*/
}

void fifo::Get(void* buf, size_t count)
{
    if (read(fileRead, buf, count) < 0)
    {
        throw ("fifo reading error");
    }
    close(fileRead);
    fileWrite = open(fifoNameToHost.c_str(), O_RDWR);
}

void fifo::Send(void* buf, size_t count)
{
    if (write(fileWrite, buf, count) < 0)
    {
        throw ("fifo writeng error");
    }
    close(fileWrite);
    fileRead = open(fifoNameToHost.c_str(), O_RDWR);
}

void fifo::Close(void)
{
/*    close(fileRead);
    close(fileWrite);*/
}

fifo::~fifo(void)
{
}

