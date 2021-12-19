//
// Created by aleksei on 28.11.2021.
//

#ifndef DAEMON_CONNECTION_H
#define DAEMON_CONNECTION_H

class Message
{
public:
  static constexpr unsigned char s_maxAliveNumber = 100;
  static constexpr unsigned char s_maxDeadNumber = 50;
private:
  unsigned char m_data = 0;
private:
  [[nodiscard]] bool isValidValue(unsigned char value) const
  {
    return  value != 0 && (alive() ? value <= s_maxAliveNumber : value <= s_maxDeadNumber);
  }
public:
  [[nodiscard]] unsigned char number() const
  {
    return m_data & 0x7F;
  }
  [[nodiscard]] bool alive() const
  {
    return m_data & 0x80;
  }

  void setNumber(unsigned char num)
  {
    if (!isValidValue(num))
    {
      m_data = 0;
    }
    else
    {
      m_data &= 0x80;
      m_data |= num;
    }
  }

  void setState(bool isAlive)
  {
    unsigned char state = isAlive ? 0x80 : 0x00;
    m_data = state | number();
  }

  [[nodiscard]] bool isValid() const
  {
    return isValidValue(number());
  }
};

class Conn
{
public:
  static Conn* create();
  virtual bool open(unsigned id) noexcept = 0;
  [[nodiscard]] virtual bool write(Message msg) const noexcept = 0;
  [[nodiscard]] virtual Message read() const noexcept = 0;
  virtual ~Conn() = default;
};


#endif //DAEMON_CONNECTION_H
