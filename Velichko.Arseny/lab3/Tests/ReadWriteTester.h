#pragma once

#include "ThreadPool.h"

#include "Mediator.h"
#include "Worker.h"

template<template<class> class Container, class ValueType>
class ReadWriteTester {
public:
	ReadWriteTester();
	~ReadWriteTester();

	void setData(const std::vector<ValueType>& data);
	void forwardData();

	void startWriters(uint32_t n = 1);
	void startReaders(uint32_t n = 1);

	void waitForTestsEnd();

	const Container<ValueType>& container() const;

private:
	static constexpr int ReaderTimeout = 5000;

	ThreadPool* m_threadPool;
	Container<ValueType>* m_container;
	Mediator<ValueType>* m_mediator;

	std::vector<ValueType> m_data;

	using WorkerCtor = std::function<Worker<ValueType>* ()>;
	void startWorkers(uint32_t n, const WorkerCtor& workerCtor);
};

#include "ReadWriteTester.tpp"
