#include "Logger.h"
#include "SignalListener.h"
#include "MultiThreading/MutexLocker.h"
#include "TimeUtils.h"

SignalListener::SignalListener(const SharedSiSet& signalSet, int timeout)
	: m_timeout(TimeUtils::msecToTimespec(timeout)), m_signalSet(signalSet) {}

SharedSiInfo SignalListener::accept() {
	MutexLocker locker(&m_mutex);
	SharedSiInfo siInfo = m_waitingSignals.front();
	m_waitingSignals.pop();
	return siInfo;
}

bool SignalListener::empty() const {
	MutexLocker locker(&m_mutex);
	return m_waitingSignals.empty();
}

void SignalListener::run() {
	log_info("Listening started");
	while (!m_isCanceled) {
		siginfo_t siInfo;
		if (sigtimedwait(m_signalSet.get(), &siInfo, &m_timeout) == -1) {
			continue;
		}

		auto sharedInfo = std::make_shared<siginfo_t>(siInfo);
		addSignal(sharedInfo);
	}
}

void SignalListener::cancel() {
	m_isCanceled = true;
}

void SignalListener::waitForSignal() {
	MutexLocker locker(&m_mutex);
	pthread_cond_wait(&m_clientNewSignal, &m_mutex);
}

void SignalListener::addSignal(const SharedSiInfo& siInfo) {
	MutexLocker locker(&m_mutex);
	m_waitingSignals.push(siInfo);
	pthread_cond_signal(&m_clientNewSignal);
}
