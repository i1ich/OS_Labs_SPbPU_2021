#pragma once

#include <pthread.h>
#include <atomic>

class Semaphore {
public:
	explicit Semaphore(int n = 0);

	void acquire(uint32_t n = 1);
	void release(uint32_t n = 1);

private:
	pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;
	std::atomic_uint32_t m_available;
};
