#pragma once

#include <map>
#include <sstream>

#include "Grammar.h"
#include "SysLogger.h"

class ConfigParser {
public:
	explicit ConfigParser(const Grammar* grammar);
	~ConfigParser();

	bool parse(const std::string& config);

	template<class T>
	T getValue(int key) const;

private:
	const Grammar* m_grammar;
	std::map<int, std::string> m_values;
};

#include "ConfigParser.tpp"