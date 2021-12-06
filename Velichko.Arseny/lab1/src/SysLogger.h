#pragma once

#include <string>
#include <syslog.h>

#include "Singleton.h"

class SysLogger : public Singleton<SysLogger> {
public:
	static void log(int priority, const std::string& msg);

private:
	explicit SysLogger(const char* ident);

	friend class Singleton<SysLogger>;
};

#define log_info(msg) SysLogger::log(LOG_INFO, msg)
#define log_warning(msg) SysLogger::log(LOG_WARNING, msg)
#define log_error(msg) SysLogger::log(LOG_ERR, msg)
#define log_errno(msg) SysLogger::log(LOG_ERR, (msg) + std::string(strerror(errno)))

#ifndef NDEBUG
#define log_debug(msg) SysLogger::log(LOG_DEBUG, msg)
#else
#define log_debug(msg)
#endif