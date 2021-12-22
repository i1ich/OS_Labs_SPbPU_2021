#include "MutexGuardLock.h"

MutexGuardLock::MutexGuardLock(pthread_mutex_t* mutex) {
    _mutex = mutex;
	pthread_mutex_lock(_mutex);
}

MutexGuardLock::~MutexGuardLock() {
	pthread_mutex_unlock(_mutex);
}