#include <algorithm>
#include <cstring>

#include "ThreadPool.h"
#include "MutexLocker.h"
#include "Logger.h"

void* ThreadPool::onStartThread(void* arg) {
	auto runnableInfo = reinterpret_cast<RunnableInfo*>(arg);
	Runnable* runnable = runnableInfo->runnable;
	ThreadPool* pool = runnableInfo->pool;

	runnable->run();
	if (runnable->autoDelete()) {
		delete runnable;
	}

	pool->removeThread(pthread_self());
	delete runnableInfo;
	return nullptr;
}

bool ThreadPool::start(Runnable* runnable) {
	pthread_t thread;
	auto info = new RunnableInfo{ this, runnable };
	int createRes = pthread_create(&thread, nullptr, onStartThread, info);

	if (createRes != 0) {
		delete info;
		log_error_desc("On thread creation occurred error", createRes);
		return false;
	}

	MutexLocker locker(&m_mutex);
	m_threads.push_back(thread);
	return true;
}

void ThreadPool::waitForDone() {
	while (activeThreadCount() != 0) {
		MutexLocker locker(&m_mutex);
		pthread_t thread = m_threads.front();
		locker.unlock();

		pthread_join(thread, nullptr);
	};
}

void ThreadPool::removeThread(pthread_t thread) {
	MutexLocker locker(&m_mutex);
	auto it = std::find(m_threads.begin(), m_threads.end(), thread);
	m_threads.erase(it);
}

size_t ThreadPool::activeThreadCount() const {
	MutexLocker locker(&m_mutex);
	return m_threads.size();
}
