//
// Created by aleksei on 20.12.2021.
//

#ifndef DAEMON_NODE_H
#define DAEMON_NODE_H

template<typename T>
struct Node
{
  T value;
  Node* next;
  explicit Node(const T& value, Node* next = nullptr)
  : value(value)
  , next(next)
  {};

  [[nodiscard]] virtual std::size_t key() const
  {
    return std::hash<T>{}(value);
  }
  virtual ~Node() = default;
};


#endif //DAEMON_NODE_H
