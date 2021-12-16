#include "GameControlBlock.h"
#include "MultiThreading/MutexLocker.h"

#include "Logger.h"

void GameControlBlock::setGameValue(int value) {
	MutexLocker locker(&m_mutex);
	m_aliveCount = 0;
	m_gameValue = value;
	pthread_cond_broadcast(&m_gameValueFetched);
}

int GameControlBlock::waitGameValue() {
	MutexLocker locker(&m_mutex);

	m_sem->release();
	pthread_cond_wait(&m_gameValueFetched, &m_mutex);
	return m_gameValue;
}

void GameControlBlock::playerJoined() {
	m_playersCount++;
}

void GameControlBlock::playerSurvived() {
	m_aliveCount++;
}

void GameControlBlock::playerLeft() {
	m_sem->release();
	m_playersCount--;
}


uint32_t GameControlBlock::aliveCount() const {
	return m_aliveCount;
}

uint32_t GameControlBlock::playersCount() const {
	return m_playersCount;
}

void GameControlBlock::waitAllPlayers() {
	m_sem->acquire(m_playersCount);
}

GameControlBlock::GameControlBlock() : m_sem(new Semaphore) {}

GameControlBlock::~GameControlBlock() {
	delete m_sem;
}
