#pragma once

#include <vector>
#include <string_view>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

#include "Singleton.h"

class Logger : public Singleton<Logger> {
public:
	enum Level {
		Level_Debug,
		Level_Info,
		Level_Warning,
		Level_Error,
		Level_WhatATerribleFailure,
		Level_Count,
	};

	void log(Level level, const std::string& message,
			 const std::string& file, const std::string& function, int line);

	static std::string errorDesc(const std::string& message, int errnum);

protected:
	Logger();
	Logger(const std::string& filename);

private:
	void initLevelNames();
	std::string_view getLevelName(Level level);

	std::shared_ptr<std::ostream> m_os;
	std::vector<std::string_view> m_levelNames;
};

#define log_macro(level, msg) \
	Logger::instance()->log(level, msg, fs::relative(__FILE__, PROJECT_DIR), __FUNCTION__, __LINE__)

#ifndef NDEBUG
#define log_debug(msg) log_macro(Logger::Level_Debug, msg)
#elif
#define log_debug(msg)
#endif

#define log_info(msg) log_macro(Logger::Level_Info, msg)
#define log_warning(msg) log_macro(Logger::Level_Warning, msg)
#define log_error(msg) log_macro(Logger::Level_Error, msg)
#define log_wtf(msg) log_macro(Logger::Level_WhatATerribleFailure, msg)

#define log_error_desc(msg, errnum) log_error(Logger::errorDesc(msg, errnum))
#define log_errno(msg) log_error_desc(msg, errno)