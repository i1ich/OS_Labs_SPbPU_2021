#include "weather_predictor.h"
#include "../utils/logger.h"

int main (int argc, char* argv[]) {
    WeatherPredictor& weatherPredictor = WeatherPredictor::getInstance();
    Logger& logger = Logger::getInstance();

    if (argc != 2) {
        logger.logError("Not enough arguments, host PID is needed");
        return -1;
    }

    int hostPid = atoi(argv[1]);

    logger.logInfo("Client is working with host (pid): " + std::to_string(hostPid));

    weatherPredictor.setHostPid(hostPid);

    if (!weatherPredictor.openConnection()) return 0;

    logger.logInfo("Started weather predictor");

    if (!weatherPredictor.initializeSeedBase()) return 0;

    weatherPredictor.run();

    return 0;
}