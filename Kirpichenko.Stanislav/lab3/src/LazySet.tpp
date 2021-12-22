template<class T, class Hasher> LazySet<T, Hasher>::Node::Node(size_t hash) {
    //_data = item;
    _hash = hash;
    _next = nullptr;
    _fDeleted = false;
    if (pthread_mutex_init(&_mutex, NULL) != 0) {
        throw std::runtime_error("Unable to initialize mutex");
    }
}

template<class T, class Hasher> LazySet<T, Hasher>::Node::~Node() {
    pthread_mutex_destroy(&_mutex);
}

template<class T, class Hasher> LazySet<T, Hasher>::LazySet(): _hasher() {
    _head = new Node(0);
    _head->_next = new Node((size_t)-1);
}



template<class T, class Hasher> bool LazySet<T, Hasher>::isValid(LazySet<T, Hasher>::Node* prev, LazySet<T, Hasher>::Node* cur) {
    return !prev->_fDeleted && !cur->_fDeleted && prev->_next == cur;
}

template<class T, class Hasher> bool LazySet<T, Hasher>::add(const T& item) {
    size_t hash = _hasher(item);
    while (true) {
        Node* prev = _head;
        Node* cur = _head->_next;
        while (cur->_hash < hash) {
            prev = cur;
            cur = cur->_next;
        }
        MutexGuardLock prevLock(&prev->_mutex);
        MutexGuardLock curLock(&cur->_mutex);
        if (isValid(prev, cur)) {
            if (cur->_hash == hash) {
                return false;
            }
            else {
                Node* node = new LazySet<T, Hasher>::Node(hash);
                node->_next = cur;
                prev->_next = node;
                return true;
            }
        }
    }
}

template<class T, class Hasher> bool LazySet<T, Hasher>::remove(const T& item) {
    size_t hash = _hasher(item);
    while (true) {
        Node* prev = _head;
        Node* cur = _head->_next;
        while (cur->_hash < hash) {
            prev = cur;
            cur = cur->_next;
        }
        MutexGuardLock prevLock(&prev->_mutex);
        MutexGuardLock curLock(&cur->_mutex);
        if (isValid(prev, cur)) {
            if (cur->_hash != hash) {
                return false;
            }
            else {
                cur->_fDeleted = true;
                prev->_next = cur->_next;
                delete cur;
                return true;
            }
        }
    }
}

template<class T, class Hasher> bool LazySet<T, Hasher>::contains(const T &item) {
    size_t hash = _hasher(item);
    Node* cur = _head->_next;
    while (cur->_hash < hash) {
        cur = cur->_next;
    }
    return cur->_hash == hash && !cur->_fDeleted;
}

template<class T, class Hasher> LazySet<T, Hasher>::~LazySet() {
    Node* cur = _head;
    Node* next = _head;
    while (next) {
        pthread_mutex_lock(&cur->_mutex);
        next = cur->_next;
        delete cur;
        cur = next;
    }
}