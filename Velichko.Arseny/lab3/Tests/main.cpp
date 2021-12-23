#include <numeric>

#include <unistd.h>

#include "Logger.h"
#include "FineGrainedSet.h"
#include "Tests.h"

int main(int argc, char* argv[]) {
	Logger::create();

	size_t dataSize = 10000;
	if (argc > 2) {
		log_error("Too many arguments");
		return EXIT_FAILURE;

	} else if (argc == 2) {
		std::istringstream iss(argv[1]);
		iss >> dataSize;
	}
	log_info("Data size: " + std::to_string(dataSize));

	std::vector<int> data(dataSize);
	std::iota(data.begin(), data.end(), 0);

	uint32_t threadsNum = sysconf(_SC_NPROCESSORS_ONLN);
	log_info("Thread number: " + std::to_string(threadsNum));

	Tests::writersTestSuite<FineGrainedSet>(data, threadsNum);
	Tests::readersTestSuite<FineGrainedSet>(data, threadsNum);
	Tests::commonTestSuite<FineGrainedSet>(data, threadsNum);

	return 0;
}