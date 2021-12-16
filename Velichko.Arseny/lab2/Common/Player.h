#pragma once

#include <atomic>

#include "Connection.h"
#include "MultiThreading/Runnable.h"
#include "GameProto.h"

using GameProto::PlayerStatus;

class Player : public Runnable {
public:
	Player(int id, int timeout, Connection::Role connRole);

	int id() const;
	PlayerStatus status() const;

protected:
	void writeMessage(const GameProto::Message& msg);
	void readMessage(GameProto::Message& msg);

	void setStatus(PlayerStatus status);

private:
	std::unique_ptr<Connection> m_conn;
	PlayerStatus m_status = GameProto::Status_Alive;
};
