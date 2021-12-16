//
// Created by aleksei on 28.11.2021.
//

#include <sys/syslog.h>
#include <csignal>
#include <semaphore.h>
#include <cstring>
#include <random>
#include <sys/stat.h>
#include <fcntl.h>
#include "Client.h"

using namespace std;

constexpr unsigned s_timeout = 5;
Client* Client::s_instance;

Client::Client()
{
  signal(SIGTERM, signalHandler);
}

void Client::run() noexcept
{
  syslog(LOG_INFO, "Client %i started", m_clientId);
  Message msg;
  msg.setState(true);
  while (m_isValid)
  {
    msg.setNumber(generateNum());

    syslog(LOG_INFO, "Client %i number: %i", m_clientId, msg.number());

    if (!m_pConnection->write(msg))
    {
      syslog(LOG_ERR, "Writing error");
      break;
    }
    sem_post(m_clientSemaphore);

    if(!wait()) break;

    msg = m_pConnection->read();
    syslog(LOG_INFO, "Client %i alive: %s", m_clientId, msg.alive() ? "alive" : "dead");
    m_isAlive = msg.alive();
  }
}

unsigned char Client::generateNum() const
{
  std::random_device rd;
  std::mt19937 rng(rd());
  int max = m_isAlive? Message::s_maxAliveNumber : Message::s_maxDeadNumber;
  std::uniform_int_distribution<int> distribution(1, max);
  return distribution(rng);
}

bool Client::wait() const
{
  struct timespec ts{};
  int rc = clock_gettime(CLOCK_REALTIME, &ts);
  if (rc == -1)
  {
    syslog(LOG_ERR, "Waiting error: %s", strerror(errno));
    return false;
  }
  ts.tv_sec += s_timeout;
  if (sem_timedwait(m_hostSemaphore, &ts) == -1)
  {
    syslog(LOG_ERR, "Expired timeout");
    return false;
  }
  return true;
}

bool Client::connect(const shared_ptr<Conn>& connection) noexcept
{
  m_pConnection = connection;

  m_hostSemaphore = sem_open(("host_" + std::to_string(m_clientId)).c_str(), O_CREAT, DEFFILEMODE, 0);
  if (m_hostSemaphore == SEM_FAILED)
  {
    syslog(LOG_ERR, "Connection failed:%s", strerror(errno));
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

void Client::signalHandler(int signal)
{
  if (signal == SIGTERM)
  {
    syslog(LOG_INFO, "Client terminated");
    exit(EXIT_SUCCESS);
  }
}

Client::~Client()
{
  if (m_hostSemaphore != SEM_FAILED)
  {
    sem_close(m_hostSemaphore);
  }
  if (m_clientSemaphore != SEM_FAILED)
  {
    sem_close(m_clientSemaphore);
  }
  syslog(LOG_INFO, "Client %i destroyed", m_clientId);
}

Client *Client::getInstance()
{
  if(s_instance == nullptr){
    s_instance = new Client();
  }
  return s_instance;
}

void Client::setClientID(unsigned int clientID)
{
  m_clientId = clientID;
}

void Client::deleteInstance()
{
  if(s_instance != nullptr){
    delete s_instance;
    s_instance = nullptr;
  }
}
