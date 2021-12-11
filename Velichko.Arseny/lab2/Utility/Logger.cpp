#include <iostream>
#include <fstream>
#include <cstring>

#include "Logger.h"

//Initialization with dummy deleter
Logger::Logger() : m_os(&std::cout, [](const std::ostream*) {}) {}

Logger::Logger(const std::string& filename) : m_os(new std::ofstream(filename)) {}

void Logger::initLevelNames() {
	if (!m_levelNames.empty()) { return; }

	m_levelNames.resize(Level_Count);

	m_levelNames[Level_Debug] = "Debug";
	m_levelNames[Level_Info] = "Info";
	m_levelNames[Level_Warning] = "Warning";
	m_levelNames[Level_Error] = "Error";
	m_levelNames[Level_WhatATerribleFailure] = "Wtf";
}

std::string_view Logger::getLevelName(Level level) {
	initLevelNames();
	return m_levelNames[level];
}

void Logger::log(Level level, const std::string& message,
				 const std::string& file, const std::string& function, int line) {
	//Concatenate msg for correct multithreaded output
	std::ostringstream completeMsg;
	completeMsg << "[" << getLevelName(level) << ": "
				<< file << ", " << function << ", " << line << "] "
				<< message << '\n';

	*m_os << completeMsg.str();
}

std::string Logger::errorDesc(const std::string& message, int errnum) {
	return message + " (" + strerror(errnum) + ")";
};
