#include "parser.h"

Parser Parser::instance;


Parser& Parser::get() { return instance; }


bool Parser::parseConfig(const std::string& configFile) {
	std::ifstream file(configFile);
	std::string str;

	if (!file.is_open()) {
        syslog(LOG_ERR, "ERROR: config file is not open");
		return false;
	}

	std::string key;
	std::string value;

	while (file >> key >> value) {
        auto search = Parser::configParametersValues.find(key);
        if (search == Parser::configParametersValues.end()) {
            syslog(LOG_ERR, "ERROR: wrong key in config");
            return false;
        }
        switch (search->second) {
            case TIME: {
                if (!isValidTime(value)) {
                    file.close();
                    return false;
                }
                fillParameters(TIME, value);
                break;
            }
            case DIR1: {
                fillParameters(DIR1, value);
                break;
            }
            case DIR2: {
                fillParameters(DIR2, value);
                break;
            }
        }
    }
	file.close();
	return true;
}


bool Parser::fillParameters(configParameters parameter, const std::string& parameterValue) {
    Parser::parameters[parameter] = parameterValue;
	return true;
}


bool Parser::isValidTime(const std::string& stringTime) {
    int time;

    try {
        time = std::stoi(stringTime);
    } catch (const std::invalid_argument &e) {
        syslog(LOG_ERR, "ERROR: time is not parseable");
        return false;
    } catch (const std::out_of_range &e) {
        syslog(LOG_ERR, "ERROR: time is out of range of int");
        return false;
    }

    if (time <= 0) {
        syslog(LOG_ERR, "ERROR: time is leq 0");
        return false;
    }

    return true;
}

