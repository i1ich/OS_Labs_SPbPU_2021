#include "Logger.h"
#include "ReadWriteTester.h"

template<template<class> class Container, class ValueType>
void Tests::writersTestSuite(const std::vector<ValueType>& data, uint32_t threadsNum) {
	ReadWriteTester<Container, ValueType> tester;

	tester.setData(data);
	tester.startWriters(threadsNum);
	tester.waitForTestsEnd();

	const auto& container = tester.container();
	for (int val : data) {
		if (!container.contains(val)) {
			log_error("Value " + std::to_string(val) + "wasn't written");
		}
	}

	log_info("Writers test finished");
}

template<template<class> class Container, class ValueType>
void Tests::readersTestSuite(const std::vector<ValueType>& data, uint32_t threadsNum) {
	ReadWriteTester<FineGrainedSet, ValueType> tester;

	tester.setData(data);
	tester.forwardData();
	tester.startReaders(threadsNum);
	tester.waitForTestsEnd();

	if (!tester.container().empty()) {
		log_error("Not all values have been read");
	}
	log_info("Readers test finished");
}

template<template<class> class Container, class ValueType>
void Tests::commonTestSuite(const std::vector<ValueType>& data, uint32_t threadsNum) {
	ReadWriteTester<FineGrainedSet, ValueType> tester;

	for (uint32_t readers = 1; readers < threadsNum; readers++) {
		for (uint32_t writers = 1; writers <= threadsNum - readers; writers++) {
			tester.setData(data);
			tester.startWriters(writers);
			tester.startReaders(readers);
			tester.waitForTestsEnd();

			if (!tester.container().empty()) {
				log_error("Test failed with " + std::to_string(readers) +
				          " readers and " + std::to_string(writers) + " writers");
			}
		}
	}

	log_info("Common test finished");
}