//
// Created by aleksei on 28.11.2021.
//

#include "conn_pipe.h"
#include <unistd.h>
#include <cstring>
#include <syslog.h>
#include <cerrno>


using namespace std;

Message Conn_pipe::read()  const noexcept
{
  Message msg;
  if (::read(m_fd[0], &msg, sizeof(Message)) < 0)
  {
    msg.setNumber(0);
  }
  return msg;
}

bool Conn_pipe::write(Message msg)  const noexcept
{
  return ::write(m_fd[1], &msg, sizeof(Message)) > 0;
}

Conn_pipe::~Conn_pipe()
{
  close(m_fd[0]);
  close(m_fd[1]);
}

bool Conn_pipe::open(unsigned) noexcept
{
  int rc = pipe(m_fd);
  if (rc == -1)
  {
    syslog(LOG_ERR, "Conn with pipe failed:%s", strerror(errno));
    return false;
  }
  return true;
}

Conn* Conn::create()
{
  return new Conn_pipe();
}