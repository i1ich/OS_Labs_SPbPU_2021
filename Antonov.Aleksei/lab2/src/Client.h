//
// Created by aleksei on 28.11.2021.
//

#ifndef DAEMON_CLIENT_H
#define DAEMON_CLIENT_H

#include "Connections/conn.h"

#include <memory>
#include <semaphore.h>

class Client
{
private:
  sem_t* m_clientSemaphore{};
  sem_t* m_hostSemaphore{};
  std::shared_ptr<Conn> m_pConnection;
  bool m_isAlive = true;
  bool m_isValid = true;
  unsigned m_clientId{};
  static Client* s_instance;
private:
  [[nodiscard]] unsigned char generateNum() const;
  [[nodiscard]] bool wait() const;
  static void signalHandler(int signal);
  explicit Client();
public:
  Client(Client &other) = delete;
  void operator=(const Client&) = delete;
  static Client *getInstance();
  static void deleteInstance();

  void setClientID(unsigned clientID);
  bool connect(const std::shared_ptr<Conn>& connection) noexcept;
  void run() noexcept;
  ~Client();
};


#endif //DAEMON_CLIENT_H
