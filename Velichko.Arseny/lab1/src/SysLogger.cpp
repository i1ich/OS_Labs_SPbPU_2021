#include "SysLogger.h"

SysLogger::SysLogger(const char* ident) {
	openlog(ident, LOG_PID, LOG_DAEMON);
}

void SysLogger::log(int priority, const std::string& msg) {
	auto instance = SysLogger::instance();

	if (instance) {
		syslog(priority, "%s", msg.c_str());
	}
}