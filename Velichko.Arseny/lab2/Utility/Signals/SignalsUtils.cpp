#include "SignalsUtils.h"

SignalUtils::SharedSiSet SignalUtils::createSiSet(const std::vector<int>& signals) {
	auto siSet = new sigset_t;
	sigemptyset(siSet);

	for (int signal : signals) {
		sigaddset(siSet, signal);
	}

	return SharedSiSet(siSet);
}
