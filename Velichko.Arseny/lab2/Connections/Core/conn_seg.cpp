#include <numeric>
#include <sys/shm.h>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

#include "conn_seg.h"

#include "MultiThreading/MutexLocker.h"
#include "TimeUtils.h"
#include "Logger.h"

ConnectionPrivate::ConnectionPrivate(Connection* conn) : m_conn(conn) {
	int flags = S_IRWXU | S_IRWXG;
	if (conn->role() == Connection::Host) {
		flags |= IPC_CREAT | IPC_EXCL;
	}

	int key = ftok(".", KeySeed + m_conn->id());
	m_sharedBuffer = new SharedBuffer(key, DefaultBufferSize);
	m_sharedBuffer->open(flags);
}

ssize_t ConnectionPrivate::read(char* data, size_t size) {
	if (!m_sharedBuffer->waitForReadyRead(m_conn->timeout())) {
		return -1;
	}
	return m_sharedBuffer->read(data, size);
}

ssize_t ConnectionPrivate::write(const char* data, size_t size) {
	return m_sharedBuffer->write(data, size);
}

bool ConnectionPrivate::isOpen() const {
	return m_sharedBuffer->isOpen();
}

ConnectionPrivate::~ConnectionPrivate() {
	if (m_conn->role() == Connection::Host) {
		m_sharedBuffer->scheduleDelete();
	}
	delete m_sharedBuffer;
}

SharedBuffer::SharedBuffer(int key, size_t size) : m_key(key), m_dataSize(size) {}

ssize_t SharedBuffer::read(char* data, size_t size) {
	if (!isOpen()) {
		log_warning("Shared buffer wasn't opened");
		return -1;
	}

	MutexLocker locker(&m_header->mutex);

	size_t readSize = std::min(m_header->pos, size);
	size_t bias = m_header->pos - readSize;
	memcpy(data, m_data + bias, readSize);

	m_header->pos -= readSize;
	return readSize;
}

ssize_t SharedBuffer::write(const char* data, size_t size) {
	if (!isOpen()) {
		log_warning("Shared buffer wasn't opened");
		return -1;
	}

	MutexLocker locker(&m_header->mutex);

	size_t emptySize = m_dataSize - m_header->pos;
	size_t writeSize = std::min(emptySize, size);
	memcpy(m_data + m_header->pos, data, writeSize);

	m_header->pos = writeSize;
	m_header->writerPid = getpid();

	pthread_cond_broadcast(&m_header->cond);
	return writeSize;
}

bool SharedBuffer::waitForReadyRead(int timeout) {
	if (!isOpen()) {
		log_warning("Shared buffer wasn't opened");
		return false;
	}

	MutexLocker locker(&m_header->mutex);
	pid_t pid = getpid();

	timespec absTimeout = TimeUtils::absTimeout(timeout);
	while (m_header->pos == 0 || m_header->writerPid == pid) {
		int waitRes = pthread_cond_timedwait(&m_header->cond, &m_header->mutex, &absTimeout);
		if (waitRes != 0) { return false; }
	}
	return true;
}

void SharedBuffer::initSharedMutex(pthread_mutex_t* mutex) {
	pthread_mutexattr_t mutexAttr;
	pthread_mutexattr_init(&mutexAttr);
	pthread_mutexattr_setpshared(&mutexAttr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mutex, &mutexAttr);
}

void SharedBuffer::initSharedCond(pthread_cond_t* cond) {
	pthread_condattr_t condAttr;
	pthread_condattr_init(&condAttr);
	pthread_condattr_setpshared(&condAttr, PTHREAD_PROCESS_SHARED);
	pthread_cond_init(cond, &condAttr);
}

SharedBuffer::~SharedBuffer() {
	SharedBuffer::close();
}

bool SharedBuffer::open(int flags) {
	m_memId = shmget(m_key, m_dataSize + sizeof(Header), flags);
	if (m_memId == -1) {
		log_errno("Allocation of shared memory failed");
		return false;
	}

	auto segPtr = shmat(m_memId, nullptr, 0);
	m_header = static_cast<Header*>(segPtr);
	m_data = static_cast<char*>(segPtr) + sizeof(Header);

	if (flags & IPC_CREAT) {
		initSharedMutex(&m_header->mutex);
		initSharedCond(&m_header->cond);

		m_header->pos = 0;
		m_header->writerPid = -1;
	}
	return IODevice::open(flags);
}

void SharedBuffer::close() {
	if (!isOpen()) { return; }
	shmdt(m_header);
	IODevice::close();
}

void SharedBuffer::scheduleDelete() {
	if (!isOpen()) { return; }
	shmctl(m_memId, IPC_RMID, nullptr);
}

