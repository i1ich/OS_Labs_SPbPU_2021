#include "Semaphore.h"
#include "MutexLocker.h"

Semaphore::Semaphore(int n) {
	m_available = n;
}

void Semaphore::acquire(uint32_t n) {
	MutexLocker locker(&m_mutex);
	while (m_available < n) {
		pthread_cond_wait(&m_cond, &m_mutex);
	}
	m_available -= n;
}

void Semaphore::release(uint32_t n) {
	MutexLocker locker(&m_mutex);
	m_available += n;
	pthread_cond_signal(&m_cond);
}