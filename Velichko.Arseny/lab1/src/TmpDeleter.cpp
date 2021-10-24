#include <syslog.h>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include <filesystem>
#include <stdexcept>

#include "TmpDeleter.h"
#include "TmpDeleterGrammar.h"
#include "SysLogger.h"

namespace fs = std::filesystem;

TmpDeleter::TmpDeleter(const std::string& config)
	: Daemon("TmpDeleter", true) {
	m_config = fs::absolute(config);

	m_parser = std::make_unique<ConfigParser>(new TmpDeleterGrammar());

	if (!fetchConfig()) {
		throw std::invalid_argument("Bad config");
	}
}

void TmpDeleter::run() {
	log_info("TmpDeleter started");

	registerHandler();

	while (!m_interruptRequested)
	{
		if (m_fetchRequested) {
			fetchConfig();
			m_fetchRequested = false;
		}

		deleteTmpFiles();
		sleep(m_interval);
	}

	m_interruptRequested = false;
	log_info("TmpDeleter interrupted");
}

void TmpDeleter::handleSignal(int signum) {
	auto instance = TmpDeleter::instance();
	switch (signum) {
		case SIGTERM:
			instance->requestInterrupt();
			break;

		case SIGHUP:
			instance->requestFetch();
			break;

		default:
			break;
	}
}

void TmpDeleter::registerHandler() {
	struct sigaction act = {};
	memset(&act, 0, sizeof(act));

	sigset_t lockMask;
	sigemptyset(&lockMask);
	sigaddset(&lockMask, SIGTERM);
	sigaddset(&lockMask, SIGHUP);
	act.sa_mask = lockMask;

	act.sa_handler = handleSignal;
	sigaction(SIGTERM, &act, nullptr);
	sigaction(SIGHUP, &act, nullptr);
}

bool TmpDeleter::fetchConfig() {
	if (!m_parser->parse(m_config)) { return false; }

	try {
		m_interval = m_parser->getValue<int>(TmpDeleterGrammar::Interval);
		m_workDir = m_parser->getValue<std::string>(TmpDeleterGrammar::WorkDirectory);

		log_debug("New interval: " + std::to_string(m_interval));
		log_debug("New work directory: " + m_workDir);

	} catch (const std::exception& e) {
		log_error(e.what());
		return false;
	}

	return true;
}

void TmpDeleter::requestInterrupt() {
	m_interruptRequested = true;
}

void TmpDeleter::requestFetch() {
	m_fetchRequested = true;
}

void TmpDeleter::deleteTmpFiles() {
	fs::directory_iterator it;
	try {
		it = fs::directory_iterator(m_workDir);
	} catch (const std::exception& e) {
		log_error(e.what());
		return;
	}

	for (; it != fs::end(it); it++) {
		if (!fs::is_regular_file(it->status())) {
			continue;
		}

		const auto& path = it->path();
		if (path.extension() == ".tmp") {
			try {
				fs::remove(path);
			} catch (const std::exception& e){
				log_warning(e.what());
				continue;
			}

			log_debug("File removed: " + path.filename().string());
		}
	}
}