//
// Created by aleksei on 28.11.2021.
//

#ifndef WOLF_GOAT_GAME_CONN_MQ_H
#define WOLF_GOAT_GAME_CONN_MQ_H

#include "conn.h"
#include <mqueue.h>

class Conn_mq : public Conn
{
private:
  mqd_t m_queueDes;
  std::string m_queueName;
public:
  [[nodiscard]] bool write(Message msg) const noexcept override;
  [[nodiscard]] Message read() const noexcept override;
  bool open(unsigned id) noexcept override;
  ~Conn_mq() override;

};


#endif //WOLF_GOAT_GAME_CONN_MQ_H
