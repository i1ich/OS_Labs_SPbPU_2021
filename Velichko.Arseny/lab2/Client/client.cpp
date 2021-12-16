#include "Logger.h"
#include "MultiThreading/ThreadPool.h"

#include "ClientPlayer.h"
#include "HandshakeHelper.h"
#include "GameDefines.h"

using namespace GameDefines;

int main(int argc, char** argv) {
	Logger::create();

	if (argc != 2) {
		log_error("Wrong arguments number");
		return EXIT_FAILURE;
	}

	int hostPid = std::stoi(argv[1]);

	HandshakeHelper helper(SIGUSR1);
	helper.sendRequest(hostPid);
	int id = helper.waitAnswer(HandshakeWaitTime);
	if (id == -1) { return EXIT_FAILURE; }

	log_info("Successfully connected to host with id: " + std::to_string(id));

	ClientPlayer* player;
	try {
		player = new ClientPlayer(id);
	} catch (const std::exception& e) {
		log_error(e.what());
		return  EXIT_FAILURE;
	}

	auto threadPool = std::make_unique<ThreadPool>();
	threadPool->start(player);
	threadPool->waitForDone();

	return 0;
}