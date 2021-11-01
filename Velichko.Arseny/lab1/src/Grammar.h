#pragma once

#include <type_traits>
#include <functional>
#include <map>
#include <string>

class Grammar {
public:
	explicit Grammar(const std::string& delimiter = "=");

	void registerToken(int key, const std::string& name);

	bool contains(const std::string& name) const;
	int getKey(const std::string& name) const;
	std::string delimiter() const;
	size_t tokensNumber() const;

private:
	std::map<std::string, int> m_tokenNames;
	std::string m_delimiter;
};
