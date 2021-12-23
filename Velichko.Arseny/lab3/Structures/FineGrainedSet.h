#pragma once

#include <functional>
#include <memory>
#include <atomic>

#include <pthread.h>

template<class Key, class Compare = std::less<Key>>
class FineGrainedSet {
public:
	FineGrainedSet();
	explicit FineGrainedSet(const Compare& comp);
	~FineGrainedSet();

	bool insert(const Key& key);
	bool erase(const Key& key);
	bool contains(const Key& key) const;

	bool empty() const;
	size_t size() const;

private:

	struct NodeBase {
		NodeBase* m_next = nullptr;
		pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;

		void lock();
		void unlock();
	};

	struct ListHeader : NodeBase {
		std::atomic_size_t m_size = 0;
	};

	struct ListNode : NodeBase {
		Key m_key;

		explicit ListNode(const Key& key);
	};

	Compare m_comp;
	mutable ListHeader m_head;

	auto findAndLock(const Key& key) const;
};

#include "FineGrainedSet.tpp"