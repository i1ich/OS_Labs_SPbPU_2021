//
// Created by aleksei on 28.11.2021.
//

#include <semaphore.h>
#include <cstring>
#include <sys/syslog.h>
#include <sys/stat.h>
#include <csignal>
#include <fcntl.h>
#include "ClientHandler.h"

using namespace std;

constexpr unsigned s_timeout = 5;

ClientHandler::ClientHandler(unsigned int clientID, pid_t clientPid)
: m_pConnection(Conn::create())
, m_clientId(clientID)
, m_pid(clientPid)
{}

void ClientHandler::run()
{
  syslog(LOG_INFO, "Client handler %i started", m_clientId);
  while (m_numOfDeaths < 2)
  {
    if (!m_isFreshData)
    {
      if(!wait()) break;
      m_data = m_pConnection->read();
      m_isFreshData = true;
    }
  }
  kill();
}

Message ClientHandler::getData()
{
  m_isFreshData = false;
  return m_data;
}

bool ClientHandler::connect() noexcept
{
  if (!m_pConnection->open(m_clientId))
  {
    syslog(LOG_ERR, "Connection failed");
    return false;
  }

  m_hostSemaphore = sem_open(("host_" + std::to_string(m_clientId)).c_str(), O_CREAT, DEFFILEMODE, 0);
  if (m_hostSemaphore == SEM_FAILED)
  {
    syslog(LOG_ERR, "Semaphore failed: %s", strerror(errno));
    return false;
  }

  m_clientSemaphore = sem_open(("client_" + std::to_string(m_clientId)).c_str(), O_CREAT, DEFFILEMODE, 0);
  if (m_clientSemaphore == SEM_FAILED)
  {
    syslog(LOG_ERR, "Connection failed:%s", strerror(errno));
    return false;
  }
  return true;
}

bool ClientHandler::hasFreshData() const
{
  return m_isFreshData;
}

bool ClientHandler::writeData(Message msg)
{
  m_numOfDeaths = msg.alive() ? 0 : m_numOfDeaths + 1;
  bool success = m_pConnection->write(msg);
  sem_post(m_hostSemaphore);
  return success;
}

ClientHandler::~ClientHandler()
{
  if (m_hostSemaphore != SEM_FAILED)
  {
    sem_unlink(("host_" + std::to_string(m_clientId)).c_str());
  }
  if (m_clientSemaphore != SEM_FAILED)
  {
    sem_unlink(("client_" + std::to_string(m_clientId)).c_str());
  }

  syslog(LOG_INFO, "Client handler %i destroyed", m_clientId);
}

void ClientHandler::kill()
{
  if (m_pid > 0 && !m_killed)
  {
    syslog(LOG_INFO, "Sending SIGTERM to client");
    m_killed = true;
    ::kill(m_pid, SIGTERM);
  }
}

bool ClientHandler::alive() const
{
  return m_numOfDeaths < 2;
}

bool ClientHandler::wait() const
{
  struct timespec ts{};
  int rc = clock_gettime(CLOCK_REALTIME, &ts);
  if (rc == -1)
  {
    syslog(LOG_ERR, "Waiting error: %s", strerror(errno));
    return false;
  }
  ts.tv_sec += s_timeout;
  if (sem_timedwait(m_clientSemaphore, &ts) == -1)
  {
    syslog(LOG_ERR, "Expired timeout");
    return false;
  }
  return true;
}

void ClientHandler::setPid(pid_t pid)
{
  m_pid = pid;
}

std::shared_ptr<Conn> ClientHandler::getConnection() const
{
  return m_pConnection;
}
