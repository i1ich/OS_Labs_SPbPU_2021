#include "MutexLocker.h"
#include "TimeUtils.h"

template<class ValueType>
void Mediator<ValueType>::put(const ValueType& val) {
	MutexLocker locker(&m_mutex);
	m_set.insert(val);
	pthread_cond_broadcast(&m_newVal);
}

template<class ValueType>
bool Mediator<ValueType>::take(const ValueType& val, int timeout) {
	MutexLocker locker(&m_mutex);

	timespec absTimeout = TimeUtils::absTimeout(timeout);
	while (m_set.find(val) == m_set.end()) {
		int waitRes = pthread_cond_timedwait(&m_newVal, &m_mutex, &absTimeout);
		if (waitRes != 0) { return false; }
	}

	m_set.erase(val);
	return true;
}
