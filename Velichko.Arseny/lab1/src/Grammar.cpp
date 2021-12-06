#include "Grammar.h"

Grammar::Grammar(const std::string& delimiter) : m_delimiter(delimiter){}

void Grammar::registerToken(int key, const std::string& name) {
	m_tokenNames[name] = key;
}

bool Grammar::contains(const std::string& name) const {
	return m_tokenNames.find(name) != m_tokenNames.end();
}

int Grammar::getKey(const std::string& name) const {
	return m_tokenNames.at(name);
}

std::string Grammar::delimiter() const {
	return m_delimiter;
}

size_t Grammar::tokensNumber() const {
	return m_tokenNames.size();
}
