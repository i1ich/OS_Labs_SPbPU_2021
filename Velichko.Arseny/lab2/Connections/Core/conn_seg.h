#pragma once

#include <vector>

#include "Connection.h"
#include "IODevice.h"

class SharedBuffer : public IODevice {
public:
	SharedBuffer(int key, size_t size);
	~SharedBuffer();

	ssize_t read(char* data, size_t size) override;
	ssize_t write(const char* data, size_t size) override;

	bool waitForReadyRead(int timeout) override;

	bool open(int flags) override;
	void close() override;

	void scheduleDelete();

private:
	struct Header {
		pthread_cond_t cond;
		pthread_mutex_t mutex;
		size_t pos;
		pid_t writerPid;
	};

	static void initSharedMutex(pthread_mutex_t* mutex);
	static void initSharedCond(pthread_cond_t* cond);

	int m_key;
	size_t m_dataSize;

	int m_memId;
	Header* m_header = nullptr;
	char* m_data = nullptr;
};

class ConnectionPrivate {
public:
	ConnectionPrivate(Connection* conn);
	~ConnectionPrivate();

	ssize_t read(char* data, size_t size);
	ssize_t write(const char* data, size_t size);

	bool isOpen() const;

private:
	static constexpr char KeySeed = 'G';
	static constexpr size_t DefaultBufferSize = 128;

	Connection* m_conn;
	SharedBuffer* m_sharedBuffer;
};
