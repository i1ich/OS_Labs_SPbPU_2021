#pragma once

#include <vector>

namespace Tests {
	template<template<class> class Container, class ValueType>
	void writersTestSuite(const std::vector<ValueType>& data, uint32_t threadsNum);

	template<template<class> class Container, class ValueType>
	void readersTestSuite(const std::vector<ValueType>& data, uint32_t threadsNum);

	template<template<class> class Container, class ValueType>
	void commonTestSuite(const std::vector<ValueType>& data, uint32_t threadsNum);
}

#include "Tests.tpp"