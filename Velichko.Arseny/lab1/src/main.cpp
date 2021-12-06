#include <iostream>

#include "TmpDeleter.h"
#include "SysLogger.h"

int main(int argc, char** argv) {
	if (argc != 2) {
		std::cerr << "Wrong number of arguments" << std::endl;
		return EXIT_FAILURE;
	}

	SysLogger::create("Daemon");

	try {
		TmpDeleter::create(argv[1]);

	} catch (const std::exception& e) {
		log_error(e.what());
		return EXIT_FAILURE;
	}

	if (!TmpDeleter::instance()->start()) {
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}