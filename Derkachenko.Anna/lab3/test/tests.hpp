#pragma once
#include "setBase.hpp"
#include <cstddef>

void writersTest(SetBase &set, size_t threads, size_t maxTestNum);
void readersTest(SetBase &set, size_t threads, size_t maxTestNum);
