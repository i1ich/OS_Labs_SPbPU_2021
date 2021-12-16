#pragma once

#include <random>

class RandGenerator {
public:
	RandGenerator(int a, int b);
	RandGenerator(int a, int b, unsigned int seed);

	void setRange(int a, int b);
	int generate();

private:
	std::default_random_engine m_randEngine;
	std::uniform_int_distribution<int> m_uniformDist;
};
