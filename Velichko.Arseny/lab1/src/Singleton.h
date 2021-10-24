#pragma once

#include <memory>

template<class T>
class Singleton {
public:
	template<class... Args>
	static void create(Args... args);

	static T* instance();

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

protected:
	Singleton() = default;

private:
	static std::unique_ptr<T> s_instance;
};

#include "Singleton.tpp"


