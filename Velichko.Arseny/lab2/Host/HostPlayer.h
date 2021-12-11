#pragma once

#include "GameControlBlock.h"
#include "Player.h"

class HostPlayer : public Player {
public:
	HostPlayer(int id, const SharedControlBlock& controlBlock);

	void run() override;

private:
	std::string logPrefix() const;
	PlayerStatus updateStatus(int gameValue, int clientValue);

	SharedControlBlock m_controlBlock;

	bool hideCondition(int diff) const;
	bool resurrectCondition(int diff) const;
};
