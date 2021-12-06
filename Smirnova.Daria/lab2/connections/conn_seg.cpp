#include <stdexcept>
#include <cstring>
#include "conn_seg.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

Connection* Connection::createConnection() {
    return new SegConnection();
}

void SegConnection::open(size_t hostPid, bool isCreator) {
    m_isCreator = isCreator;
    m_connectionName = "/tmp/lab2_seg" + std::to_string(hostPid);
    int shmFlag = 0666;
    std::ofstream ofs(m_connectionName);
    ofs.close();
    key_t shm_key = ftok(m_connectionName.c_str(), 1);
    if (shm_key == -1)
        throw std::runtime_error("shmkey failed, error " + std::string(strerror(errno)));
    if (isCreator)
        shmFlag |= IPC_CREAT;
    m_segId = shmget(shm_key, sizeof(char), shmFlag);
    if (m_segId == -1)
        throw std::runtime_error("shmget failed, error " + std::string(strerror(errno)));
    m_segMap = (char *)shmat(m_segId, nullptr, 0);
}

void SegConnection::read(void *buf, size_t count) {
    if (buf == nullptr)
        throw std::runtime_error("nullptr buf passed into seg reading");
    memcpy(buf, m_segMap, count - 1);
    if (buf == (char*) -1) {
        throw std::runtime_error("reading error " + std::string(strerror(errno)));
    }
}

void SegConnection::write(void *buf, size_t count) {
    if (buf == nullptr)
        throw std::runtime_error("nullptr buf passed into seg writing");
    memcpy(m_segMap, buf, count - 1);
    if (m_segMap == (char*) -1)
        throw std::runtime_error("writing error " + std::string(strerror(errno)));
}

void SegConnection::close() {
    if (m_isCreator && shmctl(m_segId, IPC_RMID, nullptr) < 0)
        throw std::runtime_error("close error " + std::string(strerror(errno)));
    if (shmdt(m_segMap) == -1)
        throw std::runtime_error("close error " + std::string(strerror(errno)));
    fs::remove(m_connectionName);
}