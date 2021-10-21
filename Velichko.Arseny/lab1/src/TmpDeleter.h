#pragma once

#include <atomic>

#include "Singleton.h"
#include "Daemon.h"
#include "ConfigParser.h"

class TmpDeleter : public Daemon, public Singleton<TmpDeleter> {
public:
	void requestInterrupt();
	void requestFetch();

protected:
	void run() override;

private:
	explicit TmpDeleter(const std::string& config);

	friend class Singleton<TmpDeleter>;

	static void registerHandler();
	static void handleSignal(int signum);

	bool fetchConfig();
	void deleteTmpFiles();

	std::unique_ptr<ConfigParser> m_parser;

	std::string m_config;
	unsigned int m_interval;
	std::string m_workDir;

	std::atomic_bool m_fetchRequested = false;
	std::atomic_bool m_interruptRequested = false;
};
