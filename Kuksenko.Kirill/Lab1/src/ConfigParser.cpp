#include "ConfigParser.h"

#include <iostream>

ConfigParser::ConfigParser(std::string const& config, Grammar const& grammar) : cfg(config), grammar{ grammar } {}

ConfigParser::Token::Token(std::string const& token, std::string const& value) : token{ token }, value{ value } {}

std::vector<ConfigParser::Token> ConfigParser::parse() {
    std::string delimeter = grammar.get_delimeter();

    std::vector<Token> parser_elems;

    if (cfg.is_open()) {
        for (std::string line; std::getline(cfg, line);) {
            size_t ind = line.find(delimeter);

            std::string token1 = line.substr(0, ind);
            std::string token2 = line.substr(ind + delimeter.size(), line.size());

            parser_elems.push_back(Token(token1, token2));
        }
    }

    return parser_elems;
}
