#pragma once

#include <string>

#include "Connection.h"
#include "IODevice.h"

class File : public IODevice {
public:
	explicit File(const std::string& path);

	bool open(int flags);
	void close();

	~File();

	std::string path() const;

	ssize_t read(char *data, size_t size) override;
	ssize_t write(const char *data, size_t size) override;

	bool waitForReadyRead(int timeout) override;

private:
	std::string m_path;
	int m_fd;
	bool m_isOpened = false;
};

class ConnectionPrivate {
public:
	ConnectionPrivate(Connection* conn);
	~ConnectionPrivate();

	ssize_t read(char* data, size_t size);
	ssize_t write(const char* data, size_t size);

	bool isOpen() const;

private:
	static constexpr std::string_view HOST_FIFO_PREFIX = "fifo_host_";
	static constexpr std::string_view CLIENT_FIFO_PREFIX = "fifo_client_";

	Connection* m_conn;
	File* m_inFifo;
	File* m_outFifo;
};
