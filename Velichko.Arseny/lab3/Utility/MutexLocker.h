#pragma once

#include <pthread.h>
#include <atomic>

class MutexLocker {
public:
	explicit MutexLocker(pthread_mutex_t* mutex);
	void unlock();
	~MutexLocker();

private:
	pthread_mutex_t* m_mutex;
	std::atomic_bool m_locked = true;
};
