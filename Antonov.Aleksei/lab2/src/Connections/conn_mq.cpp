//
// Created by aleksei on 28.11.2021.
//

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string>
#include <cstring>
#include <syslog.h>

#include "conn_mq.h"

constexpr char const* s_commonQueueName = "/GoatWolfQueue";
using namespace std;

bool Conn_mq::write(Message msg) const noexcept
{
  return mq_send(m_queueDes, reinterpret_cast<const char *>(&msg), sizeof(Message), 0) != -1;
}

Message Conn_mq::read() const noexcept
{
  Message msg;
  if (mq_receive(m_queueDes, reinterpret_cast<char *>(&msg), sizeof(Message), nullptr) == -1)
  {
    msg.setNumber(0);
  }
  return msg;
}

Conn_mq::~Conn_mq()
{
  mq_close(m_queueDes);
  mq_unlink(m_queueName.c_str());
}

bool Conn_mq::open(unsigned id) noexcept
{
  m_queueName = string(s_commonQueueName) + to_string(id);
  struct mq_attr attr{};
  attr.mq_flags = 0;
  attr.mq_maxmsg = 1;
  attr.mq_msgsize = sizeof(Message);
  attr.mq_curmsgs = 0;
  m_queueDes = mq_open(m_queueName.c_str(), O_RDWR | O_CREAT | O_EXCL, DEFFILEMODE, &attr);

 if (m_queueDes == -1) {
   syslog(LOG_ERR, "Conn with queue failed: %s", strerror(errno));
   return false;
 }
  return true;
}

Conn* Conn::create()
{
  return new Conn_mq();
}
