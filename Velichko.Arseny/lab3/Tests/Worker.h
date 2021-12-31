#pragma once

#include "Runnable.h"

template<class ValueType>
class Worker : public Runnable {
public:
	virtual void setData(const std::vector<ValueType>& data) = 0;
	virtual void setMediator(Mediator<ValueType>* mediator) = 0;
};