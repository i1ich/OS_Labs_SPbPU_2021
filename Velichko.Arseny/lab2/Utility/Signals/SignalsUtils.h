#pragma once

#include <csignal>
#include <memory>
#include <vector>

namespace SignalUtils {
	using SharedSiInfo = std::shared_ptr<siginfo_t>;
	using SharedSiSet = std::shared_ptr<sigset_t>;

	SharedSiSet createSiSet(const std::vector<int>& signals);
}