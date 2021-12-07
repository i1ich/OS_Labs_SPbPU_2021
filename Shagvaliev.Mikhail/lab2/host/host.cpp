#include "requestor.h"
#include "../utils/logger.h"

std::string parse_options(int argc, char* argv[]) {
    Logger& logger = Logger::getInstance();

    if (argc != 2) {
        logger.logError("Not valid number of arguments, specify dates file, example: dates.csv");
        return "";
    }

    return std::string(argv[1]);
}

int main (int argc, char* argv[]) {
    Requestor& requestor = Requestor::getInstance();
    Logger& logger = Logger::getInstance();

    auto datesFile = parse_options(argc, argv);
    if (datesFile.empty())
        return 0;

    if (!requestor.readDates(datesFile)) {
        logger.logError("Cannot read dates");
        return 0;
    }

    if (!requestor.openConnection()) return 0;

    logger.logInfo("Starting Requestor");

    if (!requestor.setUpWeatherPredictor()) return 0;

    requestor.run();

    return 0;
}
