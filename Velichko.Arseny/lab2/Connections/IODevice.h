#pragma once

#include <cstddef>

class IODevice {
public:
	virtual ssize_t read(char* data, size_t size) = 0;
	virtual ssize_t write(const char* data, size_t size) = 0;

	virtual bool open(int flags = 0);
	virtual void close();

	bool isOpen() const;
	int flags() const;

	virtual bool waitForReadyRead(int timeout) = 0;

private:
	bool m_isOpen = false;
	int m_flags = 0;
};

inline bool IODevice::open(int flags) {
	m_isOpen = true;
	m_flags = flags;
	return true;
}

inline void IODevice::close() {
	m_isOpen = false;
	m_flags = 0;
}

inline bool IODevice::isOpen() const {
	return m_isOpen;
}

inline int IODevice::flags() const {
	return m_flags;
}