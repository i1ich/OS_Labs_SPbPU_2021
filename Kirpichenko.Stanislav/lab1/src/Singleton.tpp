template <class T> std::unique_ptr<T> Singleton<T>::_instance = nullptr;

template <class T> T* Singleton<T>::getInstance() {
    if (!_instance) {
        _instance.reset(new T());
    }
    return _instance.get();
}
