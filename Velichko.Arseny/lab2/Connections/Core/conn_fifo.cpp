#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdexcept>

#include "conn_fifo.h"
#include "Logger.h"

ConnectionPrivate::ConnectionPrivate(Connection* conn) : m_conn(conn) {
	std::string idStr = std::to_string(conn->id());

	std::string inFifoName = std::string(CLIENT_FIFO_PREFIX) + idStr;
	std::string outFifoName = std::string(HOST_FIFO_PREFIX) + idStr;

	if (m_conn->role() == Connection::Host) {
		mkfifo(inFifoName.c_str(), S_IRWXU | S_IRWXG);
		mkfifo(outFifoName.c_str(), S_IRWXU | S_IRWXG);
		std::swap(inFifoName, outFifoName);
	}

	m_inFifo = new File(inFifoName);
	m_outFifo = new File(outFifoName);

	m_inFifo->open(O_RDWR | O_NONBLOCK);
	m_outFifo->open(O_RDWR | O_NONBLOCK);
}

ssize_t ConnectionPrivate::read(char* data, size_t size) {
	if (!m_inFifo->waitForReadyRead(m_conn->timeout())) {
		return -1;
	}
	return m_inFifo->read(data, size);
}

ssize_t ConnectionPrivate::write(const char* data, size_t size) {
	return m_outFifo->write(data, size);
}

bool ConnectionPrivate::isOpen() const {
	return m_inFifo->isOpen() && m_outFifo->isOpen();
}

File::File(const std::string& path) : m_path(path) {}

bool File::open(int flags) {
	m_fd = ::open(m_path.c_str(), flags);
	if (m_fd == -1) {
		log_errno("File open failed");
		return false;
	}
	return IODevice::open(flags);
}

void File::close() {
	if (!isOpen()) { return; }
	::close(m_fd);
	IODevice::close();
}

File::~File() {
	File::close();
}

ssize_t File::read(char* data, size_t size) {
	if (!isOpen()) {
		log_warning("File wasn't opened");
		return -1;
	}

	return ::read(m_fd, data, size);
}

ssize_t File::write(const char* data, size_t size) {
	if (!isOpen()) {
		log_warning("File wasn't opened");
		return -1;
	}

	return ::write(m_fd, data, size);
}

bool File::waitForReadyRead(int timeout) {
	if (!isOpen()) {
		log_warning("File wasn't opened");
		return false;
	}

	pollfd pollHandler = { m_fd, POLLIN };
	return poll(&pollHandler, 1, timeout) > 0;
}

std::string File::path() const {
	return m_path;
}

ConnectionPrivate::~ConnectionPrivate() {
	if (m_conn->role() == Connection::Host) {
		unlink(m_inFifo->path().c_str());
		unlink(m_outFifo->path().c_str());
	}
	delete m_inFifo;
	delete m_outFifo;
}