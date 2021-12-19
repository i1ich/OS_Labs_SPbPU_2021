//
// Created by aleksei on 28.11.2021.
//

#include "conn_shm.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <syslog.h>
#include <string>
#include <sys/stat.h>


using namespace std;
constexpr char const* s_shmCommonObjectName = "/GoatWolfSHMObject";

Message Conn_shm::read()  const noexcept
{
  return *m_address;
}

bool Conn_shm::write(Message msg)  const noexcept
{
  memcpy(m_address, &msg, sizeof(Message));
  return true;
}

Conn_shm::~Conn_shm()
{
  munmap(static_cast<void*>(m_address), sizeof(Message));
  unlink(m_shmObjectName.c_str());
  syslog(LOG_INFO,"Connection closed");
}

bool Conn_shm::open(unsigned id) noexcept
{
  int fd = -1;
  m_shmObjectName = s_shmCommonObjectName + std::to_string(id);
  fd = shm_open(m_shmObjectName.c_str(), O_CREAT | O_RDWR, DEFFILEMODE);
  if (fd == -1)
  {
    syslog(LOG_ERR, "Creation connection with shared memory failed:%s", strerror(errno));
    return false;
  }
  ftruncate(fd, sizeof(Message));
  m_address = static_cast<Message *>(mmap(nullptr, sizeof(Message), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
  if (m_address == MAP_FAILED)
  {
    syslog(LOG_ERR, "Conn with shared memory failed:%s", strerror(errno));
    return false;
  }
  return true;
}

Conn* Conn::create()
{
  return new Conn_shm();
}