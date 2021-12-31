template<class Key, class Compare>
FineGrainedSet<Key, Compare>::FineGrainedSet() : m_comp(Compare()) {}

template<class Key, class Compare>
FineGrainedSet<Key, Compare>::FineGrainedSet(const Compare& comp) : m_comp(comp) {}

template<class Key, class Compare>
FineGrainedSet<Key, Compare>::~FineGrainedSet() {
	while (m_head.m_next) {
		auto node = static_cast<ListNode*>(m_head.m_next);
		m_head.m_next = node->m_next;
		delete node;
	}
}

template<class Key, class Compare>
bool FineGrainedSet<Key, Compare>::insert(const Key& key) {
	NodeBase* prev = findAndLock(key);

	auto next = static_cast<ListNode*>(prev->m_next);
	if (next && next->m_key == key) {
		prev->unlock();
		return false;
	}

	auto node = new ListNode(key);
	node->m_next = next;
	prev->m_next = node;
	prev->unlock();

	m_head.m_size++;
	return true;
}

template<class Key, class Compare>
bool FineGrainedSet<Key, Compare>::erase(const Key& key) {
	NodeBase* prev = findAndLock(key);

	auto next = static_cast<ListNode*>(prev->m_next);
	if (!next || next->m_key != key) {
		prev->unlock();
		return false;
	}

	prev->m_next = next->m_next;
	delete next;
	prev->unlock();

	m_head.m_size--;
	return true;
}

template<class Key, class Compare>
bool FineGrainedSet<Key, Compare>::contains(const Key& key) const {
	NodeBase* prev = findAndLock(key);
	bool contains = (prev->m_next != nullptr);
	prev->unlock();
	return contains;
}

template<class Key, class Compare>
bool FineGrainedSet<Key, Compare>::empty() const {
	return size() == 0;
}

template<class Key, class Compare>
size_t FineGrainedSet<Key, Compare>::size() const {
	return m_head.m_size;
}

template<class Key, class Compare>
auto FineGrainedSet<Key, Compare>::findAndLock(const Key& key) const {
	NodeBase* curr = &m_head;
	curr->lock();

	while (auto next = static_cast<ListNode*>(curr->m_next)) {
		next->lock();
		if (m_comp(next->m_key, key)) {
			curr->unlock();
			curr = next;

		} else {
			next->unlock();
			break;
		}
	}
	return curr;
}

template<class Key, class Compare>
void FineGrainedSet<Key, Compare>::NodeBase::lock() {
	pthread_mutex_lock(&m_mutex);
}

template<class Key, class Compare>
void FineGrainedSet<Key, Compare>::NodeBase::unlock() {
	pthread_mutex_unlock(&m_mutex);
}

template<class Key, class Compare>
FineGrainedSet<Key, Compare>::ListNode::ListNode(const Key& key) : m_key(key) {}