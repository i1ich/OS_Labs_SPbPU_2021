#include <iostream>

#include "Logger.h"
#include "MultiThreading/MutexLocker.h"

#include "HostPlayer.h"
#include "GameEngine.h"
#include "GameDefines.h"

using namespace GameDefines;

GameEngine::GameEngine() :
	m_gameTimer(new Timer),
	m_randGenerator(new RandGenerator(HostMinValue, HostMaxValue)),
	m_controlBlock(new GameControlBlock),
	m_gameThreadPool(new ThreadPool) {

	auto siSet = SignalUtils::createSiSet({ SIGUSR1, SIGINT, SIGTERM });
	m_signalListener = new SignalListener(siSet);
	m_signalListener->setAutoDelete(false);

	ThreadPool::setBlockMask(siSet);

	auto makeTurnCallback = [this] () {
		makeTurn();
	};
	m_gameTimer->callOnTimeout(makeTurnCallback);
}

void GameEngine::exec() {
	m_gameThreadPool->start(m_signalListener);
	m_signalListener->waitForSignal();

	log_info("Game started");
	int turnsWithoutAlive = 0;
	m_playerId = 0;
	m_gameTimer->start(TurnInterval);

	while (turnsWithoutAlive < TurnsWithoutAlive) {
		if (!processSignals()) { break; }
		m_controlBlock->waitAllPlayers();

		int gameValue = m_randGenerator->generate();
		log_info("Generated game value: " + std::to_string(gameValue));
		m_controlBlock->setGameValue(gameValue);
		waitTurn();

		uint32_t aliveCount = m_controlBlock->aliveCount();
		log_info("Players alive: " + std::to_string(aliveCount));
		if (aliveCount == 0) {
			turnsWithoutAlive++;
		} else {
			turnsWithoutAlive = 0;
		}
	}

	m_controlBlock->setGameValue(EndGameValue);
	m_signalListener->cancel();

	log_info("Game finished");
}

bool GameEngine::processSignals() {
	while (!m_signalListener->empty()) {
		auto siInfo = m_signalListener->accept();

		int signo = siInfo->si_signo;
		if (signo == SIGINT || signo == SIGTERM) {
			return false;
		}

		int clientPid = siInfo->si_value.sival_int;

		int playerId = addPlayer();
		if (playerId == -1) { continue; }

		sigval answerVal = {};
		answerVal.sival_int = playerId;
		if (sigqueue(clientPid, SIGUSR1, answerVal) == -1) {
			log_errno("Failed to send answer to client");
		}
	}
	return true;
}

void GameEngine::makeTurn() {
	MutexLocker locker(&m_mutex);
	pthread_cond_signal(&m_canMakeTurn);
}

GameEngine::~GameEngine() {
	delete m_gameTimer;
	delete m_randGenerator;

	m_gameThreadPool->waitForDone();
	delete m_gameThreadPool;
	delete m_signalListener;
}

void GameEngine::waitTurn() {
	MutexLocker locker(&m_mutex);
	pthread_cond_wait(&m_canMakeTurn, &m_mutex);
}

int GameEngine::addPlayer() {
	HostPlayer* player;
	try {
		player = new HostPlayer(m_playerId, m_controlBlock);

	} catch (const std::exception& e) {
		log_error(e.what());
		return -1;
	}

	if (!m_gameThreadPool->start(player)) {
		log_error("Failed to start new player");
		return -1;
	}

	m_controlBlock->playerJoined();
	return m_playerId++;
}

