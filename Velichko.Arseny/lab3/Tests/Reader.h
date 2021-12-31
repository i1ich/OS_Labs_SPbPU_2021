#pragma once

#include <vector>

#include "Worker.h"
#include "Mediator.h"

template<template<class> class Container, class ValueType>
class Reader : public Worker<ValueType> {
public:
	Reader(int timeout, Container<ValueType>* container);

	void setData(const std::vector<ValueType>& data) override;
	void setMediator(Mediator<ValueType>* mediator) override;

	void run() override;

private:
	int m_timeout;

	Container<ValueType>* m_container;
	Mediator<ValueType>* m_mediator = nullptr;
	std::vector<ValueType> m_data;

	pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
};

#include "Reader.tpp"