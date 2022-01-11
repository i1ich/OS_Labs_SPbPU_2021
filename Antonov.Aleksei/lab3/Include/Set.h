//
// Created by aleksei on 20.12.2021.
//

#ifndef WOLF_GOAT_GAME_SET_H
#define WOLF_GOAT_GAME_SET_H

template <typename T>
class Set
{
public:
  virtual bool add(const T& item) = 0;
  virtual bool remove(const T& item) = 0;
  virtual bool contains(const T& item) = 0;
  virtual bool empty() = 0;

  virtual ~Set() = default;
};
#endif //WOLF_GOAT_GAME_SET_H
