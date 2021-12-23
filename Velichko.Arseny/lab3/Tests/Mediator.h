#pragma once

#include <set>

#include <pthread.h>

template<class ValueType>
class Mediator {
public:
	void put(const ValueType& val);
	bool take(const ValueType& val, int timeout);

private:
	std::set<ValueType> m_set;
	mutable pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
	mutable pthread_cond_t m_newVal = PTHREAD_COND_INITIALIZER;
};

#include "Mediator.tpp"