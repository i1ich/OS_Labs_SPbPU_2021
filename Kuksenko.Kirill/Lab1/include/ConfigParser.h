#pragma once

#include <fstream>
#include <string>
#include <vector>

class Grammar {
private:
    std::string const empty_string = "";

    std::vector<std::string> const tokens;
    std::string const delimeter = "=";

public:
    Grammar(std::vector<std::string> const& tokens) : tokens{ tokens } {}
    Grammar(Grammar const& other) : tokens{ other.tokens } {}
    ~Grammar() = default;

    std::string const& get_token(size_t ind) const {
        if (ind < tokens.size()) {
            return tokens[ind];
        }

        return empty_string;
    }

    std::string const& get_delimeter() const {
        return delimeter;
    }
};

class ConfigParser {
public:
    struct Token {
        Token(std::string const& token, std::string const& value);
        Token(Token const& other) = default;

        std::string const token;
        std::string const value;
    };

private:
    std::ifstream cfg;
    Grammar grammar;

public:
    ConfigParser(std::string const& config, Grammar const& grammar);
    ~ConfigParser() = default;

    std::vector<ConfigParser::Token> parse();
};
