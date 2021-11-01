#include "../include/Parser.h"

Parser& Parser::getInstance() {
    static Parser parser;
    return parser;
}

int Parser::getTimeInterval() {
    return std::any_cast<int>(parameters[CONFIG_PARAMETERS::TIME_INTERVAL]);
}

std::vector<DirectoryWithIgnFile> Parser::getDirectories() {
    return std::any_cast<std::vector<DirectoryWithIgnFile>>(parameters[CONFIG_PARAMETERS::DIRECTORIES]);
}

bool Parser::parseConfig(const std::string &configFile)  {
    cleanParameters();
    auto& logger = Logger::getInstance();
    std::ifstream file(configFile);

    if (not file.is_open()) {
        logger.logError("can not open config file");
        return false;
    }

    for (std::string line; getline(file, line);) {
        auto spaceIdx = line.find(' ');
        if (spaceIdx > line.length()) {
            logger.logError("not valid config file");
            return false;
        }

        auto key = line.substr(0, spaceIdx);
        auto val = line.substr(spaceIdx+1);

        if (key == Parser::TIME_NAME_IN_CONFIG) {
            if (getTimeInterval() > 0) {
                logger.logError("only one time interval is allowed");
                return false;
            }

            try {
                parameters[CONFIG_PARAMETERS::TIME_INTERVAL] = std::stoi(val);
            }
            catch (const std::invalid_argument& e) {
                logger.logError("not valid time interval");
                return false;
            }
        }
        else {
            getDirectoriesRef().push_back(DirectoryWithIgnFile(key, val));
        }
    }
    file.close();

    if (getTimeInterval() <= 0) {
        logger.logError("not valid time interval or no time interval in config");
        return false;
    }
    return true;
}

std::vector<DirectoryWithIgnFile>& Parser::getDirectoriesRef() {
    return std::any_cast<std::vector<DirectoryWithIgnFile>&>(parameters[CONFIG_PARAMETERS::DIRECTORIES]);
}

void Parser::cleanParameters() {
    parameters[CONFIG_PARAMETERS::TIME_INTERVAL] = 0;
    getDirectoriesRef().clear();
}