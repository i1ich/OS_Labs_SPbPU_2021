template<class T> SCSPQueue<T>::Node::Node(const T &item): _data(item) {
}


template<class T> SCSPQueue<T>::SCSPQueue() {
    _head = new Node(T());
    _tail.store(_head);
}

template<class T> T SCSPQueue<T>::dequeue() {
    Node* node = _head.load();
    if (node == _tail.load()) {
        throw std::runtime_error("Dequeue of the empty queue");
    }
    _head.store(node->_next);
    T res = node->_data;
    delete node;
    return res;
}

template<class T> void SCSPQueue<T>::enqueue(const T& item) {
    Node* node = new Node(0);
    Node* prevTail = _tail.load();
    prevTail->_data = item;
    prevTail->_next = node;
    _tail.store(node);
}

template<class T> bool SCSPQueue<T>::empty() {
    return _head.load() == _tail.load();
}

template<class T> SCSPQueue<T>::~SCSPQueue() {
    Node* node = _head.load();
    while (node) {
        _head.store(node->_next);
        delete node;
        node = _head.load();
    }
}