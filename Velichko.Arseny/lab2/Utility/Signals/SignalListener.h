#pragma once

#include <queue>
#include <atomic>

#include "Singleton.h"
#include "MultiThreading/Runnable.h"
#include "SignalsUtils.h"

using SignalUtils::SharedSiSet;
using SignalUtils::SharedSiInfo;

class SignalListener : public Runnable {
public:
	explicit SignalListener(const SharedSiSet& siSet, int timeout = 1000);

	SharedSiInfo accept();
	bool empty() const;

	void cancel();
	void run() override;

	void waitForSignal();

private:
	void addSignal(const SharedSiInfo& siInfo);

	mutable pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t m_clientNewSignal = PTHREAD_COND_INITIALIZER;
	std::atomic_bool m_isCanceled = false;

	SharedSiSet m_signalSet;
	std::queue<SharedSiInfo> m_waitingSignals;
	timespec m_timeout;
};
