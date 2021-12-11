#pragma once

#include <ctime>

namespace TimeUtils {
	timespec msecToTimespec(int msec);
	timespec absTimeout(int msec);
}