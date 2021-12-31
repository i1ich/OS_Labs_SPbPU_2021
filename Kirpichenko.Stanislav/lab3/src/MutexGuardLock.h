#pragma once
#include <pthread.h>

class MutexGuardLock {
	pthread_mutex_t* _mutex;
public:
	MutexGuardLock(pthread_mutex_t* mutex);
	~MutexGuardLock();
};