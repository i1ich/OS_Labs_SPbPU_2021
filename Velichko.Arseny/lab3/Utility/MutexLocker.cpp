#include "MutexLocker.h"

MutexLocker::MutexLocker(pthread_mutex_t* mutex) : m_mutex(mutex) {
	pthread_mutex_lock(m_mutex);
}

MutexLocker::~MutexLocker() {
	unlock();
}

void MutexLocker::unlock() {
	if (m_locked) {
		pthread_mutex_unlock(m_mutex);
		m_locked = false;
	}
}
