#include <cmath>
#include <pthread.h>

#include "Timer.h"
#include "MultiThreading/MutexLocker.h"
#include "TimeUtils.h"

Timer::Timer() {
	sigevent timeoutEvent = {};
	timeoutEvent.sigev_notify = SIGEV_THREAD;
	timeoutEvent.sigev_notify_function = onTimeout;
	timeoutEvent.sigev_value.sival_ptr = this;

	timer_create(CLOCK_REALTIME, &timeoutEvent, &m_id);
}

void Timer::onTimeout(union sigval val) {
	auto timer = reinterpret_cast<Timer*>(val.sival_ptr);
	MutexLocker locker(&timer->m_mutex);
	timer->m_callback();
}

void Timer::callOnTimeout(const Timer::Callback& callback) {
	MutexLocker locker(&m_mutex);
	m_callback = callback;
}

void Timer::start(int interval) {
	itimerspec timerSpec = {};
	timerSpec.it_value = TimeUtils::msecToTimespec(interval);
	timerSpec.it_interval = timerSpec.it_value;

	timer_settime(m_id, 0, &timerSpec, nullptr);
}

Timer::~Timer() {
	timer_delete(m_id);
}

void Timer::stop() {
	itimerspec timerSpec = {};
	timer_settime(m_id, 0, &timerSpec, nullptr);
}