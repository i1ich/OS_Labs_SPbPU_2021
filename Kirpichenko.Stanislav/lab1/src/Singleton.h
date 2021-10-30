#pragma once
#include <memory>

template <class T> class Singleton {
private:
    static std::unique_ptr<T> _instance;

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton& d) = delete;    
protected:
    Singleton() = default;
public:
    static T* getInstance();
};

#include "Singleton.tpp"
