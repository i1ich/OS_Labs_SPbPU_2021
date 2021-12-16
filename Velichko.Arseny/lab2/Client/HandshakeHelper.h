#pragma once

#include "Signals/SignalsUtils.h"

using SignalUtils::SharedSiSet;

class HandshakeHelper {
public:
	explicit HandshakeHelper(int handshakeSignal);

	void sendRequest(int hostPid) const;
	int waitAnswer(int timeout) const;

private:
	int m_handshakeSignal;
	SharedSiSet m_siSet;
};
