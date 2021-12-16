//
// Created by aleksei on 28.11.2021.
//

#ifndef DAEMON_HOST_H
#define DAEMON_HOST_H

#include <vector>
#include "ClientHandler.h"

class Host
{
private:
  std::vector<std::unique_ptr<ClientHandler>> m_clients;
  unsigned m_numOfKilled = 0;
private:
  void game();
  [[nodiscard]] bool round();
  static char generateNum();
  [[nodiscard]] bool proceedInput(Message answer, char ownNumber,  unsigned clientIdx);
  [[nodiscard]] bool createClient(unsigned idx);
public:
  [[nodiscard]] bool connectClients(unsigned clientsNum);
  void start();
  ~Host();
};


#endif //DAEMON_HOST_H
