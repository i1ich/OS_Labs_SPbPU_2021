#pragma once

#include <memory>

template<class T>
class Singleton {
public:
	template<class... Args>
	static void create(Args&&... args);

	static T* instance();

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

protected:
	Singleton() = default;

private:
	struct Wrapper : public T {
		template<class... Args>
		Wrapper(Args&&... args);
	};
	static std::unique_ptr<Wrapper> s_instance;
};

#include "Singleton.tpp"


