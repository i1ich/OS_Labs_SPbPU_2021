#include <utility>

template<class T>
std::unique_ptr<T> Singleton<T>::s_instance;

template<class T>
template<class... Args>
void Singleton<T>::create(Args... args) {
	auto ptr = new T(std::forward<Args>(args)...);
	s_instance = std::unique_ptr<T>(ptr);
}

template<class T>
T* Singleton<T>::instance() {
	return s_instance.get();
}
