template <class T> T* Singleton<T>::getInstance() {
    if (!_instance) {
        _instance.reset(new Instance());
    }
    return _instance.get();
}
