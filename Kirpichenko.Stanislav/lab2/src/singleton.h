#pragma once
#include <memory>

template <class T> class Singleton {
    struct Instance: public T {
        Instance() : T() {}
        ~Instance() = default;
    };

    inline static std::unique_ptr<Instance> _instance = nullptr;
protected:
    Singleton() = default;
    ~Singleton() = default;
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton& d) = delete;

    static T* getInstance();
};

#include "singleton.tpp"
