template<template<class> class Container, class ValueType>
Reader<Container, ValueType>::Reader(int timeout, Container<ValueType>* container) :
	m_timeout(timeout), m_container(container) {}

template<template<class> class Container, class ValueType>
void Reader<Container, ValueType>::setData(const std::vector<ValueType>& data) {
	MutexLocker locker(&m_mutex);
	m_data = data;
}

template<template<class> class Container, class ValueType>
void Reader<Container, ValueType>::setMediator(Mediator<ValueType>* mediator) {
	MutexLocker locker(&m_mutex);
	m_mediator = mediator;
}

template<template<class> class Container, class ValueType>
void Reader<Container, ValueType>::run() {
	if (!m_mediator) {
		log_error("Mediator required");
		return;
	}

	for (const auto& val : m_data) {
		if (!m_mediator->take(val, m_timeout)) {
			log_error("Take value " + std::to_string(val) + " timeout expired");
			continue;
		}
		m_container->erase(val);
	}
}