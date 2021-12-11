#include <Logger.h>

#include "HostPlayer.h"
#include "GameEngine.h"

#include "GameProto.h"
#include "GameDefines.h"

using namespace GameDefines;

HostPlayer::HostPlayer(int id, const SharedControlBlock& controlBlock) :
	Player(id, HostWaitTime, Connection::Host), m_controlBlock(controlBlock) {}

void HostPlayer::run() {
	GameProto::Message msg = {};
	int gameValue;

	log_info(logPrefix() + "started");

	do {
		try {
			gameValue = m_controlBlock->waitGameValue();

			readMessage(msg);
			int clientValue = msg.data.clientValue;
			log_info(logPrefix() + "received from client: " + std::to_string(clientValue));

			if (gameValue == EndGameValue) {
				msg.state |= GameProto::GameFinished;
			}

			PlayerStatus newStatus = updateStatus(gameValue, clientValue);
			msg.data.playerStatus = newStatus;
			writeMessage(msg);

			if (newStatus == GameProto::Status_Alive) {
				m_controlBlock->playerSurvived();
			}

		} catch (std::exception& e) {
			log_error(logPrefix() + "error occurred (" + e.what() + ")");
			break;
		}
	} while (gameValue != EndGameValue);

	m_controlBlock->playerLeft();
	log_info(logPrefix() + "finished");
}

PlayerStatus HostPlayer::updateStatus(int gameValue, int clientValue) {
	using namespace GameProto;

	PlayerStatus currStatus = status();
	PlayerStatus newStatus = currStatus;

	int diff = abs(gameValue - clientValue);

	if (currStatus == Status_Alive && !hideCondition(diff)) {
		newStatus = Status_Dead;
		log_info(logPrefix() + "died");

	} else if (currStatus == Status_Dead && resurrectCondition(diff)) {
		newStatus = Status_Alive;
		log_info(logPrefix() + "resurrected");
	}

	setStatus(newStatus);
	return newStatus;
}

bool HostPlayer::hideCondition(int diff) const {
	return diff <= HideCondValue / m_controlBlock->playersCount();
}

bool HostPlayer::resurrectCondition(int diff) const {
	return diff <= ResurrectCondValue / m_controlBlock->playersCount();
}

std::string HostPlayer::logPrefix() const {
	return "{Player " + std::to_string(id()) + "} ";
}
