#ifndef PARSER_H
#define PARSER_H

#include "definitions.h"

class Parser {
public:
	enum configParameters {
		TIME = 0,
		DIR1 = 1,
		DIR2 = 2
	};

	static bool parseConfig(const std::string& configFile);

	static std::map<configParameters, std::string> getParameters() { return parameters; }


private:
	static bool fillParameters(configParameters parameter, const std::string& parameterValue);

    static const std::map<std::string, configParameters> configParametersValues;

    static bool isValidTime(const std::string& stringTime);

    static std::map<configParameters, std::string> parameters;
	
};

#endif //PARSER_H