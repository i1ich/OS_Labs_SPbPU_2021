#include "Writer.h"
#include "Reader.h"

template<template<class> class Container, class ValueType>
ReadWriteTester<Container, ValueType>::ReadWriteTester() :
	m_threadPool(new ThreadPool),
	m_container(new Container<ValueType>),
	m_mediator(new Mediator<ValueType>) {}

template<template<class> class Container, class ValueType>
ReadWriteTester<Container, ValueType>::~ReadWriteTester() {
	delete m_container;
	delete m_threadPool;
	delete m_mediator;
}

template<template<class> class Container, class ValueType>
void ReadWriteTester<Container, ValueType>::setData(const std::vector<ValueType>& data) {
	m_data = data;
}

template<template<class> class Container, class ValueType>
void ReadWriteTester<Container, ValueType>::startWorkers(uint32_t n, const WorkerCtor& workerCtor) {
	size_t chunkSize = (m_data.size() + n - 1) / n;

	auto chunkBegin = m_data.begin();
	for (uint32_t i = 0; i < n; i++) {
		auto chunkEnd = std::min(chunkBegin + chunkSize, m_data.end());
		std::vector<ValueType> chunk(chunkBegin, chunkEnd);

		auto worker = workerCtor();
		worker->setData(chunk);
		worker->setMediator(m_mediator);

		m_threadPool->start(worker);

		chunkBegin = chunkEnd;
	}
}

template<template<class> class Container, class ValueType>
void ReadWriteTester<Container, ValueType>::startWriters(uint32_t n) {
	startWorkers(n, [this] {
		return new Writer<Container, ValueType>(m_container);
	});
}

template<template<class> class Container, class ValueType>
void ReadWriteTester<Container, ValueType>::startReaders(uint32_t n) {
	startWorkers(n, [this] {
		return new Reader<Container,ValueType>(ReaderTimeout, m_container);
	});
}

template<template<class> class Container, class ValueType>
const Container<ValueType>& ReadWriteTester<Container, ValueType>::container() const {
	return *m_container;
}

template<template<class> class Container, class ValueType>
void ReadWriteTester<Container, ValueType>::waitForTestsEnd() {
	m_threadPool->waitForDone();
}

template<template<class> class Container, class ValueType>
void ReadWriteTester<Container, ValueType>::forwardData() {
	for (const auto& val : m_data) {
		m_container->insert(val);
		m_mediator->put(val);
	}
}
