#include "RandGenerator.h"

RandGenerator::RandGenerator(int a, int b) : RandGenerator(a, b, std::random_device()()) {}

RandGenerator::RandGenerator(int a, int b, unsigned int seed) :
	m_randEngine(seed), m_uniformDist(a, b) {}

int RandGenerator::generate() {
	return m_uniformDist(m_randEngine);
}

void RandGenerator::setRange(int a, int b) {
	m_uniformDist = std::uniform_int_distribution<int>(a, b);
}
