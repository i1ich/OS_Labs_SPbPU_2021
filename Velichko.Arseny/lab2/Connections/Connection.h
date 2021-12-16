#pragma once

#include <memory>
#include <cstdio>

class ConnectionPrivate;

class Connection {
public:
	enum Role {
		Client,
		Host,
	};

	Connection(int id, int timeout, Role role);
	~Connection();

	int id() const;
	int timeout() const;
	bool isOpen() const;

	Role role() const;

	ssize_t read(char* data, size_t size);
	ssize_t write(const char* data, size_t size);

private:
	//Constant propagation
	const ConnectionPrivate* pImpl() const;
	ConnectionPrivate* pImpl();

	int m_id;
	int m_timeout;
	Role m_role;
	std::unique_ptr<ConnectionPrivate> m_pImpl;
};