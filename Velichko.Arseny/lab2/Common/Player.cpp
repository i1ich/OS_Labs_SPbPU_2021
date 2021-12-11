#include <stdexcept>

#include "Player.h"

Player::Player(int id, int timeout, Connection::Role connRole) :
	m_conn(new Connection(id, timeout, connRole)) {

	if (!m_conn->isOpen()) {
		throw std::runtime_error("Connection open failed");
	}
}

int Player::id() const {
	return m_conn->id();
}

PlayerStatus Player::status() const {
	return m_status;
}

void Player::setStatus(PlayerStatus status) {
	m_status = status;
}

void Player::writeMessage(const GameProto::Message& msg) {
	ssize_t size = sizeof(GameProto::Message);
	auto data = reinterpret_cast<const char*>(&msg);

	if (m_conn->write(data, size) < size) {
		throw std::runtime_error("Message write failed");
	}
}

void Player::readMessage(GameProto::Message& msg) {
	ssize_t size = sizeof(GameProto::Message);
	auto data = reinterpret_cast<char*>(&msg);

	if (m_conn->read(data, size) < size) {
		throw std::runtime_error("Message read failed");
	}
}
