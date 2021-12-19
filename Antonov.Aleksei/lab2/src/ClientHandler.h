//
// Created by aleksei on 28.11.2021.
//

#ifndef WOLF_GOAT_GAME_CLIENTHANDLER_H
#define WOLF_GOAT_GAME_CLIENTHANDLER_H


#include <semaphore.h>
#include <memory>
#include "Connections/conn.h"


class ClientHandler
{
private:
  sem_t* m_clientSemaphore = nullptr;
  sem_t* m_hostSemaphore = nullptr;
  std::shared_ptr<Conn> m_pConnection;
  bool m_isFreshData = false;
  unsigned m_clientId = 0;
  Message m_data;
  size_t m_numOfDeaths = 0;
  pid_t m_pid = 0;
  bool m_killed = false;
private:
  [[nodiscard]] bool wait() const;
public:
  explicit ClientHandler(unsigned clientID, pid_t clientPid = 0);
  void run();
  void setPid(pid_t pid);
  [[nodiscard]] bool hasFreshData() const;
  Message getData();
  [[nodiscard]] bool writeData(Message msg);
  [[nodiscard]] bool connect() noexcept;
  [[nodiscard]] std::shared_ptr<Conn> getConnection() const;
  void kill();
  [[nodiscard]] bool alive() const;
  ~ClientHandler();
};


#endif //WOLF_GOAT_GAME_CLIENTHANDLER_H
