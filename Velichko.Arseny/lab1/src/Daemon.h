#pragma once

#include <string>

class Daemon {
public:
	explicit Daemon(const std::string& name, bool pidLock = false,
					int umask = 0, const std::string& execDir = "/");

	bool start();

	static bool closeHandler(int handler);
	static bool isPidRunning(pid_t pid);
	static bool changeDir(const std::string& path);

	virtual ~Daemon() = default;

protected:
	virtual void run() = 0;

private:
	bool createLockFile() const;
	bool overwritePid(pid_t& previous) const;
	bool lockPid() const;

	bool m_pidLock;
	std::string m_pidLockPath;

	std::string m_name;
	pid_t m_pid;
	int m_umask;
	std::string m_execDir;
};
