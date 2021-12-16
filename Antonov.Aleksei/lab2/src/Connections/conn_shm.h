//
// Created by aleksei on 28.11.2021.
//

#ifndef DAEMON_SHM_CONNECTION_H
#define DAEMON_SHM_CONNECTION_H

#include "conn.h"

#include <string>

class Conn_shm : public Conn
{
private:
   Message* m_address = nullptr;
   std::string m_shmObjectName;
public:
  [[nodiscard]] Message read() const noexcept override;
  [[nodiscard]] bool write(Message msg) const noexcept override;
  bool open(unsigned id) noexcept override;
  ~Conn_shm() override;
};


#endif //DAEMON_SHM_CONNECTION_H
