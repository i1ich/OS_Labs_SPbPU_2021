#pragma once

#include <pthread.h>
#include <list>

#include "Runnable.h"
#include "Signals/SignalsUtils.h"

using SignalUtils::SharedSiSet;

class ThreadPool {
public:
	bool start(Runnable* runnable);
	void waitForDone();

	size_t activeThreadCount() const;
	static void setBlockMask(const SharedSiSet& set);

private:
	static void* onStartThread(void* arg);
	void removeThread(pthread_t thread);

	struct RunnableInfo {
		ThreadPool* pool;
		Runnable* runnable;
	};

	mutable pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
	std::list<pthread_t> m_threads;
};
