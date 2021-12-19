#ifndef __HOST_H_
#define __HOST_H_

#include <sys/types.h>
#include <atomic>
#include <vector>
#include <mutex>
#include <stdlib.h>
#include <string.h>
#include <bits/types/siginfo_t.h>

using byte = unsigned char;

struct session_data {
  static const int BULK_SIZE = 256;

  std::atomic<pid_t> clientPid;
  std::atomic<bool> needSend;
  byte bulkToSend[BULK_SIZE];
  std::atomic<bool> needRead;
  byte bulkToRead[BULK_SIZE];
  std::atomic<bool> termClient;

  session_data() : clientPid(-1), needSend(false), needRead(false), termClient(false)
  {
    memset(bulkToSend, 0, BULK_SIZE);
    memset(bulkToRead, 0, BULK_SIZE);
  }

  session_data(const session_data &other)
  {
    clientPid = (pid_t)other.clientPid;
    needSend = (bool)other.needSend;
    needRead = (bool)other.needRead;
    termClient = (bool)other.termClient;
    memcpy(bulkToSend, other.bulkToSend, BULK_SIZE);
    memcpy(bulkToRead, other.bulkToRead, BULK_SIZE);
  }
};

class host {
public:
  host(const host& host) = delete;
  host& operator=(const host&) = delete;

  static void Run(void);
  static void Term(void);

private:
  host(void);
  static void signalHandler(int signum, siginfo_t* info, void *ptr);
  static void ClientSession(pid_t clientPid);

  static host* hostInstance;
  std::vector<session_data> sessions;
  std::mutex sessionsMutex;
  std::atomic<pid_t> waitingClient;
};

#endif //!__HOST_H_
