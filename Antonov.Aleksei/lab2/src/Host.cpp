//
// Created by aleksei on 28.11.2021.
//

#include <ctime>
#include <unistd.h>
#include <sys/syslog.h>
#include <pthread.h>
#include <random>
#include "Host.h"
#include "Client.h"

using namespace std;

namespace GameConstants
{
  constexpr char s_maxWolfNum = 100;
  constexpr char s_aliveGoatNumerator = 70;
  constexpr char s_deadGoatNumerator = 20;
}

bool Host::createClient(unsigned idx)
{
  pid_t pid = fork();
  auto& clientHandler = m_clients.back();
  if (pid == -1)
  {
    syslog(LOG_ERR, "Fork failed");
    return false;
  }
  else if (pid == 0)
  {
    auto client = Client::getInstance();
    client->setClientID(idx);
    if (client->connect(clientHandler->getConnection()))
    {
      client->run();
    }
    else
    {
      syslog(LOG_ERR, "Connection client failed");
      return false;
    }
    Client::deleteInstance();
    exit(EXIT_SUCCESS);
  }
  else
  {
    clientHandler->setPid(pid);
  }
  return true;
}

bool Host::connectClients(unsigned int clientsNum)
{
  syslog(LOG_INFO, "Connecting clients started");
  bool isOk = true;
  for (unsigned int i = 0; isOk && i < clientsNum; i++)
  {
    m_clients.emplace_back(make_unique<ClientHandler>(i));
    if (!m_clients.back()->connect())
    {
      syslog(LOG_ERR, "Connection client handler failed");
      return false;
    }
    isOk = createClient(i);
  }
  syslog(LOG_INFO, "Connecting clients finished");
  return isOk;
}

static void* startClient(void * client)
{
  static_cast<ClientHandler*>(client)->run();
  return nullptr;
}
void Host::start()
{
  syslog(LOG_INFO, "Starting the game");
  for(auto& client : m_clients)
  {
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    void* (*function)(void *) = startClient;
    pthread_create(&tid, &attr, function, client.get());
  }
  game();
}

void Host::game()
{
  unsigned roundNum = 0;
  do
  {
    syslog(LOG_NOTICE, "Round %i", roundNum++);
  } while (m_numOfKilled < m_clients.size() && round());
  syslog(LOG_NOTICE, "Game finished");
}

bool Host::round()
{
  bool isAllProcessed = false;
  vector<bool> processed(m_clients.size(), false);

  auto hostNum = generateNum();
  syslog(LOG_INFO, "Host number: %i", hostNum);


  while (!isAllProcessed)
  {
    m_numOfKilled = 0;
    isAllProcessed = true;
    for (unsigned i = 0; i < processed.size(); i++)
    {
      if(!processed[i] && m_clients[i]->alive() && m_clients[i]->hasFreshData())
      {
        auto data = m_clients[i]->getData();
        if (!data.isValid())
        {
          syslog(LOG_ERR, "Host got invalid data from client: %i", i);
          return false;
        }
        if (!proceedInput(data, hostNum, i))
        {
          syslog(LOG_ERR, "Host can't proceed input. Writing error");
          return false;
        }
        processed[i] = true;
      }
      if(!m_clients[i]->alive())
      {
        m_numOfKilled++;
        processed[i] = true;
      }
      isAllProcessed = isAllProcessed && processed[i];
    }
  }
  return true;
}

char Host::generateNum()
{
  random_device rd;
  mt19937 rng(rd());
  uniform_int_distribution<char> uni(1, GameConstants::s_maxWolfNum);
  return uni(rng);
}

bool Host::proceedInput(Message answer, char ownNumber, unsigned clientIdx)
{
  bool isAlive;
  if (answer.alive())
  {
    isAlive = abs(ownNumber - answer.number()) <= (GameConstants::s_aliveGoatNumerator/ static_cast<double>(m_clients.size()));
  }
  else
  {
    isAlive = abs(ownNumber - answer.number()) <= (GameConstants::s_deadGoatNumerator / static_cast<double>(m_clients.size()));
  }
  answer.setState(isAlive);
  return m_clients[clientIdx]->writeData(answer);
}

Host::~Host()
{
  for(auto& client: m_clients)
    client->kill();
}
