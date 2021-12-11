#pragma once

#include "Player.h"
#include "RandGenerator.h"
#include "Connection.h"

class ClientPlayer : public Player {
public:
	explicit ClientPlayer(int id);

	void run() override;

private:
	void updateStatus(PlayerStatus newStatus);

	RandGenerator* m_randGenerator;
};
