#include <fstream>
#include <regex>

#include "ConfigParser.h"

ConfigParser::ConfigParser(const Grammar* grammar) : m_grammar(grammar) {}

bool ConfigParser::parse(const std::string& config) {
	std::ifstream input(config);
	if (!input.is_open()) {
		log_error("Config didn't open: " + config);
		return false;
	}

	m_values.clear();

	const auto& delimiter = m_grammar->delimiter();
	std::regex spaceRegex("\\s*" + delimiter + "\\s*");

	std::string line;
	while (std::getline(input, line)) {
		auto it = std::sregex_token_iterator(line.begin(), line.end(), spaceRegex, -1);
		const auto& tokenName = it->str();

		if (m_grammar->contains(tokenName)) {
			int key = m_grammar->getKey(tokenName);
			const auto& tokenValue = std::next(it)->str();
			m_values[key] = tokenValue;

		} else {
			log_error("Unknown token in config: " + tokenName);
			return false;
		}
	}

	if (m_values.size() < m_grammar->tokensNumber()) {
		log_error("Config has not enough tokens: " + config);
		return false;
	}

	return true;
}

ConfigParser::~ConfigParser() {
	delete m_grammar;
}
