#pragma once

#include "MultiThreading/ThreadPool.h"
#include "MultiThreading/Runnable.h"
#include "Signals/SignalListener.h"
#include "RandGenerator.h"
#include "Timer.h"

#include "GameControlBlock.h"

using SignalUtils::SharedSiInfo;

class GameEngine {
public:
	GameEngine();
	~GameEngine();

	void exec();

private:
	bool processSignals();
	int addPlayer();

	void makeTurn();
	void waitTurn();

	int m_playerId;
	Timer* m_gameTimer;
	SharedControlBlock m_controlBlock;
	ThreadPool* m_gameThreadPool;

	SignalListener* m_signalListener;
	RandGenerator* m_randGenerator;

	pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_cond_t m_canMakeTurn = PTHREAD_COND_INITIALIZER;
};
