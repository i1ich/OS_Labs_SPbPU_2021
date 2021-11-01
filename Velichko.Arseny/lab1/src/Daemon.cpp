#include <unistd.h>
#include <sys/stat.h>
#include <filesystem>
#include <fstream>
#include <csignal>
#include <cstring>

#include "Daemon.h"
#include "SysLogger.h"

namespace fs = std::filesystem;

Daemon::Daemon(const std::string& name, bool pidLock, int umask, const std::string& execDir)
		: m_pidLock(pidLock), m_name(name), m_umask(umask), m_execDir(execDir) {

	if (pidLock) {
		m_pidLockPath = fs::absolute( PID_DIRECTORY + name + ".pid");
	}
}

bool Daemon::start() {
	pid_t pid = fork();

	if (pid < 0) {
		log_errno("First fork error: ");
		return false;
	}

	if (pid > 0) { return true; }

	if (setsid() < 0) {
		log_errno("New session error: ");
		return false;
	}

	pid = fork();

	if (pid < 0) {
		log_errno("Second fork error: ");
		return false;
	}

	if (pid > 0) { return true; }

	m_pid = getpid();
	if (m_pidLock && !lockPid()) {
		return false;
	}

	closeHandler(STDIN_FILENO);
	closeHandler(STDOUT_FILENO);
	closeHandler(STDERR_FILENO);

	umask(m_umask);

	if (!changeDir(m_execDir)) { return false; }

	run();
	return true;
}

bool Daemon::lockPid() const {
	if(!fs::exists(m_pidLockPath)) {
		return createLockFile();
	}

	pid_t cachedPid;
	if (!overwritePid(cachedPid)) { return false; };

	if (isPidRunning(cachedPid)) {
		kill(cachedPid, SIGTERM);
	}

	return true;
}

bool Daemon::isPidRunning(pid_t pid) {
	return fs::exists("/proc/" + std::to_string(pid));
}

bool Daemon::createLockFile() const {
	std::ofstream lockFile(m_pidLockPath);
	if (!lockFile) {
		log_error("PID lock file wasn't created: " + m_pidLockPath);
		return false;
	}

	lockFile << m_pid;
	return true;
}

bool Daemon::overwritePid(pid_t& previous) const {
	std::fstream lockFile(m_pidLockPath);
	if (!lockFile.is_open()) {
		log_error("Failed to overwrite PID lock file: " + m_pidLockPath);
		return false;
	}

	lockFile >> previous;
	lockFile.seekg(0, std::ios_base::beg);
	lockFile << m_pid;

	return true;
}

bool Daemon::closeHandler(int handler) {
	if (close(handler) < 0) {
		log_errno("Close handler #" + std::to_string(handler) + " error: ");
		return false;
	}
	return true;
}

bool Daemon::changeDir(const std::string& path) {
	if (chdir(path.c_str()) < 0) {
		log_errno("Failed to change directory: " + path);
		return false;
	}
	return true;
}