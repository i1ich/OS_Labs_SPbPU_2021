#include "Connection.h"
#include CONN_IMPL_HEADER

Connection::Connection(int id, int timeout, Role role)
	: m_id(id), m_timeout(timeout), m_role(role), m_pImpl(new ConnectionPrivate(this)) {}

const ConnectionPrivate* Connection::pImpl() const {
	return m_pImpl.get();
}

ConnectionPrivate* Connection::pImpl() {
	return m_pImpl.get();
}

ssize_t Connection::read(char* data, size_t size) {
	return pImpl()->read(data, size);
}

ssize_t Connection::write(const char* data, size_t size) {
	return pImpl()->write(data, size);
}

int Connection::id() const {
	return m_id;
}

int Connection::timeout() const {
	return m_timeout;
}

Connection::Role Connection::role() const {
	return m_role;
}

bool Connection::isOpen() const {
	return pImpl()->isOpen();
}

Connection::~Connection() = default;
