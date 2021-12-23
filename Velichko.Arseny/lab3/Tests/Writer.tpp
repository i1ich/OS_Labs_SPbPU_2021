template<template<class> class Container, class ValueType>
Writer<Container, ValueType>::Writer(Container<ValueType>* container) : m_container(container) {}

template<template<class> class Container, class ValueType>
void Writer<Container, ValueType>::setData(const std::vector<ValueType>& data) {
	MutexLocker locker(&m_mutex);
	m_data = data;
}

template<template<class> class Container, class ValueType>
void Writer<Container, ValueType>::setMediator(Mediator<ValueType>* mediator) {
	MutexLocker locker(&m_mutex);
	m_mediator = mediator;
}

template<template<class> class Container, class ValueType>
void Writer<Container, ValueType>::run() {
	for (const auto& val : m_data) {
		m_container->insert(val);

		if (m_mediator) {
			m_mediator->put(val);
		}
	}
}