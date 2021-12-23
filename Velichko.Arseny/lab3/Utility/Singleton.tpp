#include <utility>

template<class T>
std::unique_ptr<typename Singleton<T>::Wrapper> Singleton<T>::s_instance;

template<class T>
template<class... Args>
void Singleton<T>::create(Args&&... args) {
	s_instance = std::make_unique<Wrapper>(std::forward<Args>(args)...);
}

template<class T>
T* Singleton<T>::instance() {
	return s_instance.get();
}

template<class T>
template<class... Args>
Singleton<T>::Wrapper::Wrapper(Args&&... args) : T(std::forward<Args>(args)...) {}
