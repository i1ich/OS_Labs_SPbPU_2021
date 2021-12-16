//
// Created by aleksei on 28.11.2021.
//

#ifndef WOLF_GOAT_GAME_CONN_PIPE_H
#define WOLF_GOAT_GAME_CONN_PIPE_H

#include "conn.h"

class Conn_pipe : public Conn
{
private:
  int m_fd[2] = {0};
public:
  [[nodiscard]] Message read() const noexcept override;
  [[nodiscard]] bool write(Message msg) const noexcept override;
  bool open(unsigned id) noexcept override;
  ~Conn_pipe() override;
};


#endif //WOLF_GOAT_GAME_CONN_PIPE_H
