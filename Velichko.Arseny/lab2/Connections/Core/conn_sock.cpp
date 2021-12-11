#include <poll.h>
#include <stdexcept>
#include <unistd.h>

#include "Logger.h"
#include "conn_sock.h"

ConnectionPrivate::ConnectionPrivate(Connection* conn)
	: m_conn(conn), m_socket(new UdpSocket) {
	const std::string& idStr = 	std::to_string(conn->id());

	UnixAddress sendAddr = std::string(HOST_ADDR_PREFIX) + idStr;
	UnixAddress receiveAddr = std::string(CLIENT_ADDR_PREFIX) + idStr;

	if (m_conn->role() == Connection::Host) {
		std::swap(sendAddr, receiveAddr);
	}

	m_socket->open();
	m_socket->bind(receiveAddr);
	m_socket->setHost(sendAddr);
}

ssize_t ConnectionPrivate::read(char* data, size_t size) {
	if (!m_socket->waitForReadyRead(m_conn->timeout())) {
		return -1;
	}
	return m_socket->read(data, size);
}

ssize_t ConnectionPrivate::write(const char* data, size_t size) {
	return m_socket->write(data, size);
}

bool ConnectionPrivate::isOpen() const {
	return m_socket->isOpen();
}

ConnectionPrivate::~ConnectionPrivate() {
	delete m_socket;
}

bool UdpSocket::bind(const UnixAddress& address) {
	if (!isOpen()) {
		log_warning("Socket wasn't opened");
		return false;
	}

	if (::bind(m_socketFd, address.rawAddress(), address.length()) == -1) {
		log_errno("Socket bind failed");
		return false;
	}
	m_bindAddress = address;
	return true;
}

bool UdpSocket::waitForReadyRead(int timeout) {
	if (!isOpen()) {
		log_warning("Socket wasn't opened");
		return false;
	}

	pollfd pollHandler = { m_socketFd, POLLIN };
	return poll(&pollHandler, 1, timeout) > 0;
}

ssize_t UdpSocket::write(const char* data, size_t size) {
	if (!isOpen()) {
		log_warning("Socket wasn't opened");
		return -1;
	}

	if (m_hostAddress.isNull()) {
		log_warning("Host wasn't set");
		return -1;
	}

	return sendto(m_socketFd, data, size, MSG_DONTWAIT,
				  m_hostAddress.rawAddress(), m_hostAddress.length());
}

ssize_t UdpSocket::read(char* data, size_t size) {
	if (!isOpen()) {
		log_warning("Socket wasn't opened");
		return -1;
	}

	return recvfrom(m_socketFd, data, size, MSG_DONTWAIT, nullptr, nullptr);
}

void UdpSocket::setHost(const UnixAddress& address) {
	m_hostAddress = address;
}

UdpSocket::~UdpSocket() {
	UdpSocket::close();
}

bool UdpSocket::open(int flags) {
	if (isOpen()) {
		log_warning("Socket already open");
		return false;
	}

	m_socketFd = socket(PF_UNIX, SOCK_DGRAM, 0);
	if (m_socketFd == -1) {
		log_errno("Socket creation failed");
		return false;
	}
	return IODevice::open(flags);
}

void UdpSocket::close() {
	if (!isOpen()) { return; }

	::close(m_socketFd);
	unlink(m_bindAddress.name().c_str());
	IODevice::close();
}

UnixAddress::UnixAddress(const std::string& name) {
	setName(name);
}

const sockaddr* UnixAddress::rawAddress() const {
	return std::reinterpret_pointer_cast<sockaddr>(m_socketAddr).get();
}

size_t UnixAddress::length() const {
	return sizeof(m_socketAddr->sun_family) + strlen(m_socketAddr->sun_path);
}

void UnixAddress::setName(const std::string& name) {
	m_socketAddr.reset(new sockaddr_un());
	m_socketAddr->sun_family = AF_UNIX;
	strcpy(m_socketAddr->sun_path, name.c_str());
}

std::string UnixAddress::name() const {
	return m_socketAddr->sun_path;
}

bool UnixAddress::isNull() const {
	return !static_cast<bool>(m_socketAddr);
}
