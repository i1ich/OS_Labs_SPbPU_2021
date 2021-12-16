#include <stdexcept>

#include "ClientPlayer.h"
#include "GameProto.h"
#include "GameDefines.h"
#include "Logger.h"

using namespace GameDefines;

ClientPlayer::ClientPlayer(int id) :
	Player(id, ClientWaitTime, Connection::Client),
	m_randGenerator(new RandGenerator(AliveMinValue, AliveMaxValue)) {}

void ClientPlayer::run() {
	GameProto::Message msg = {};

	log_info("Player started");

	while (true) {
		try {
			int clientValue = m_randGenerator->generate();
			log_info("Generated client value: " + std::to_string(clientValue));

			msg.data.clientValue = clientValue;
			writeMessage(msg);

			readMessage(msg);
			if (msg.state & GameProto::GameFinished) { break; }

			PlayerStatus status = msg.data.playerStatus;
			log_info("Received status: " + std::to_string(status));
			updateStatus(status);

		} catch (std::exception& e) {
			log_error(e.what());
			break;
		}
	}

	log_info("Player finished");
}

void ClientPlayer::updateStatus(PlayerStatus newStatus) {
	if (status() == newStatus) { return; }

	switch (newStatus) {
		case GameProto::Status_Alive:
			m_randGenerator->setRange(AliveMinValue, AliveMaxValue);
			break;

		case GameProto::Status_Dead:
			m_randGenerator->setRange(DeadMinValue, DeadMaxValue);
			break;

		default:
			log_wtf("Unknown player status");
			break;
	}
}