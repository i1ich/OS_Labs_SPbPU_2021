#pragma once

#include <memory>
#include <pthread.h>
#include <atomic>

#include "MultiThreading/Semaphore.h"

class GameControlBlock {
public:
	//Game engine interface
	GameControlBlock();
	~GameControlBlock();

	void playerJoined();
	void waitAllPlayers();
	void setGameValue(int value);

	uint32_t aliveCount() const;
	uint32_t playersCount() const;

	//Player interface
	void playerSurvived();
	void playerLeft();
	int waitGameValue();

private:
	pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t m_gameValueFetched = PTHREAD_COND_INITIALIZER;
	Semaphore* m_sem;

	int m_gameValue;
	std::atomic_uint32_t m_playersCount = 0;
	std::atomic_uint32_t m_aliveCount = 0;
};

using SharedControlBlock = std::shared_ptr<GameControlBlock>;
