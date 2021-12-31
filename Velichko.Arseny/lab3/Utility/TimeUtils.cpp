#include "TimeUtils.h"

timespec TimeUtils::msecToTimespec(int msec) {
	timespec timeSpec = {};
	timeSpec.tv_sec = msec / 1'000; //msec to sec conversion
	timeSpec.tv_nsec = msec % 1'000 * 1'000'000; //msec to nsec conversion
	return timeSpec;
}

timespec TimeUtils::absTimeout(int msec) {
	timespec absTimeout = {};
	clock_gettime(CLOCK_REALTIME, &absTimeout);

	timespec relTimout = msecToTimespec(msec);
	absTimeout.tv_sec += relTimout.tv_sec;
	absTimeout.tv_nsec += relTimout.tv_nsec;

	return absTimeout;
}
