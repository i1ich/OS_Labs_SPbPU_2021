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

    static Parser& get();

    bool parseConfig(const std::string& configFile);

    std::map<configParameters, std::string> getParameters() { return parameters; }


private:
    Parser() {
        configParametersValues = {
                {"time", Parser::TIME},
                {"dirFrom", Parser::DIR1},
                {"dirTo", Parser::DIR2}
        };
    }

    Parser(Parser const&) = delete;

    Parser & operator=(Parser const&) = delete;

    bool fillParameters(configParameters parameter, const std::string& parameterValue);

    bool isValidTime(const std::string& stringTime);


    static Parser instance;

    std::map<std::string, configParameters> configParametersValues;

    std::map<configParameters, std::string> parameters;
	
};

#endif //PARSER_H