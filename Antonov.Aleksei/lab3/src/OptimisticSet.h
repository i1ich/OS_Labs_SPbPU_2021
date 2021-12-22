//
// Created by aleksei on 20.12.2021.
//

#ifndef OS_LABS_SPBPU_2021_OPTIMISTICSET_H
#define OS_LABS_SPBPU_2021_OPTIMISTICSET_H

#include <functional>
#include <cstring>
#include <iostream>
#include <optional>

#include "Set.h"
#include "Node.h"

template<typename T>
class OptimisticSet : public Set<T>
{
private:
  class SyncNode : public Node<T>
  {
  private:
    pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
    bool m_isDummy = false;
    size_t m_hash;
    size_t m_counter = 0;
    bool m_shouldBeDestroyed = false;
  public:
    static SyncNode *getDummy()
    {
      auto node = new SyncNode();
      node->m_isDummy = true;
      return node;
    }

    explicit SyncNode(const T &item = T(), SyncNode *next = nullptr) noexcept
      : Node<T>(item, next)
    {};

    [[nodiscard]] bool initMutex() noexcept
    {
      if (pthread_mutex_init(&m_mutex, nullptr) != 0) {
        std::cout << "Error in mutex initializing " << strerror(errno) << std::endl;
        return false;
      }
      return true;
    }

    void lock()
    {
      pthread_mutex_lock(&m_mutex);
    }

    void unlock()
    {
      pthread_mutex_unlock(&m_mutex);
    }

    [[nodiscard]] bool dummy() const
    {
      return m_isDummy;
    }

    void setHash(size_t value)
    {
      m_hash = value;
    }

    [[nodiscard]] virtual std::size_t key() const
    {
      return dummy() ? m_hash : std::hash<T>{}(Node<T>::value);
    }

    void inc()
    {
      m_counter++;
    }
    void dec()
    {
      m_counter--;
      if (m_shouldBeDestroyed && m_counter == 0)
        delete this;
    }

    void destroy()
    {
      m_shouldBeDestroyed = true;
    }

    ~SyncNode()
    {
      unlock();
      pthread_mutex_destroy(&m_mutex);
    }
  };
private:
  SyncNode* m_head;
private:
bool validate(SyncNode const* prev, SyncNode const* curr);
void setNode(SyncNode *& left, SyncNode* right);
public:
  OptimisticSet();
  virtual bool add(const T &item);

  virtual bool remove(const T &item);

  virtual bool contains(const T &item);

  virtual bool empty();

  ~OptimisticSet();

};
template<typename T>
OptimisticSet<T>::OptimisticSet()
{
  m_head = SyncNode::getDummy();
  m_head->setHash(0);
  m_head->next = SyncNode::getDummy();
  static_cast<SyncNode*>(m_head->next)->setHash(SIZE_MAX);
}
template<typename T>
bool OptimisticSet<T>::validate(OptimisticSet::SyncNode const *prev, OptimisticSet::SyncNode const *curr)
{
  SyncNode* node = m_head;
  node->inc();
  while (node->key() <= prev->key())
  {
    if (node->value == prev->value)
      return prev->next == curr;
    setNode(node, static_cast<SyncNode*>(node->next));
  }
  node->dec();
  return false;
}


template<typename T>
bool OptimisticSet<T>::add(const T &item)
{
  const auto key = std::hash<T>{}(item);
  std::optional<bool> result;
  while (!result)
  {
    auto prev = m_head;
    prev->inc();
    auto cur = static_cast<decltype(prev)>(m_head->next);
    cur->inc();
    while (cur->key() < key)
    {
      setNode(prev, cur);
      setNode(cur, static_cast<SyncNode*>(cur->next));
    }
    prev->lock();
    cur->lock();

    if (validate(prev, cur))
    {
      if (cur->key() == key)
      {
        result = false;
      }
      else
      {
        result = true;

        auto node = new SyncNode(item, cur);
        if (!node->initMutex())
          result = false;
        prev->next = node;
      }
    }
    prev->unlock();
    cur->unlock();
    prev->inc();
    cur->dec();
  }
  return result.value();
}


template<typename T>
bool OptimisticSet<T>::remove(const T &item)
{
  const auto key = std::hash<T>{}(item);
  std::optional<bool> result;
  while (!result)
  {
    auto prev = m_head;
    prev->inc();
    auto cur = static_cast<decltype(prev)>(m_head->next);
    cur->inc();
    while (cur->key() < key)
    {
      setNode(prev, cur);
      setNode(cur, static_cast<SyncNode*>(cur->next));
    }
    prev->lock();
    cur->lock();
    if (validate(prev, cur))
    {
      if (cur->key() == key)
      {
        prev->next = cur->next;
        cur->destroy();
        result = true;
      }
      else
      {
        result = false;
      }
    }
    cur->unlock();
    prev->unlock();
    prev->dec();
    cur->dec();
  }
  return result.value();
}

template<typename T>
bool OptimisticSet<T>::contains(const T &item)
{
  const auto key = std::hash<T>{}(item);
  std::optional<bool> result;
  while (!result)
  {
    auto prev = m_head;
    prev->inc();
    auto cur = static_cast<decltype(prev)>(m_head->next);
    cur->inc();
    while (cur->key() < key)
    {
      setNode(prev, cur);
      setNode(cur, static_cast<SyncNode*>(cur->next));
    }
    prev->lock();
    cur->lock();

    if (validate(prev, cur))
    {
      result = !cur->dummy() && cur->key() == key;
    }
    prev->unlock();
    cur->unlock();
    prev->dec();
    cur->dec();
  }
  return result.value();
}

template<typename T>
bool OptimisticSet<T>::empty()
{
  return !m_head->next->next;
}

template<typename T>
OptimisticSet<T>::~OptimisticSet()
{
  auto* node = static_cast<SyncNode*>(m_head->next);
  while (node)
  {
    delete m_head;
    m_head = node;
    node = static_cast<SyncNode*>(node->next);
  }
}

template<typename T>
void OptimisticSet<T>::setNode(OptimisticSet::SyncNode *&left, OptimisticSet::SyncNode *right)
{
  left->dec();
  left = right;
  right->inc();
}


#endif //OS_LABS_SPBPU_2021_OPTIMISTICSET_H
