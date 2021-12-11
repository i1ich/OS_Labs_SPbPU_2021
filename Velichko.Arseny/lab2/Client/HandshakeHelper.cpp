#include <unistd.h>

#include "HandshakeHelper.h"
#include "MultiThreading/ThreadPool.h"
#include "TimeUtils.h"
#include "Logger.h"

HandshakeHelper::HandshakeHelper(int handshakeSignal)
	: m_handshakeSignal(handshakeSignal) {
	m_siSet = SignalUtils::createSiSet({ handshakeSignal });
	ThreadPool::setBlockMask(m_siSet);
}

void HandshakeHelper::sendRequest(int hostPid) const {
	sigval val = {};
	val.sival_int = getpid();
	sigqueue(hostPid, m_handshakeSignal, val);
}

int HandshakeHelper::waitAnswer(int timeout) const {
	siginfo_t sigInfo;
	timespec timespec = TimeUtils::msecToTimespec(timeout);
	if (sigtimedwait(m_siSet.get(), &sigInfo, &timespec) == -1) {
		log_error("Host signal timeout expired");
		return -1;
	}
	return sigInfo.si_value.sival_int;
}
