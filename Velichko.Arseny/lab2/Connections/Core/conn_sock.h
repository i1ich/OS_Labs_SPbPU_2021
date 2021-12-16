#pragma once

#include <string_view>
#include <sys/socket.h>
#include <sys/un.h>
#include <memory>

#include "Connection.h"
#include "IODevice.h"

class UnixAddress {
public:
	UnixAddress() = default;
	UnixAddress(const std::string& name);

	bool isNull() const;

	void setName(const std::string& name);

	std::string name() const;
	const sockaddr* rawAddress() const;
	size_t length() const;

private:
	std::shared_ptr<sockaddr_un> m_socketAddr;
};

class UdpSocket : public IODevice {
public:
	~UdpSocket();

	void setHost(const UnixAddress& address);

	bool bind(const UnixAddress& address);
	bool waitForReadyRead(int timeout) override;

	ssize_t write(const char* data, size_t size) override;
	ssize_t read(char* data, size_t size) override;

	bool open(int flags = 0) override;
	void close() override;

private:
	int m_socketFd = -1;
	UnixAddress m_hostAddress;
	UnixAddress m_bindAddress;
};

class ConnectionPrivate {
public:
	ConnectionPrivate(Connection* conn);
	~ConnectionPrivate();

	ssize_t read(char* data, size_t size);
	ssize_t write(const char* data, size_t size);

	bool isOpen() const;

private:
	static constexpr std::string_view HOST_ADDR_PREFIX = "sock_host_";
	static constexpr std::string_view CLIENT_ADDR_PREFIX = "sock_client_";

	Connection* m_conn;
	UdpSocket* m_socket;
};
