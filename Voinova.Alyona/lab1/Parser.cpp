//
// Created by pikabol88 on 10/22/21.
//

#include <sys/syslog.h>
#include "Parser.h"

Parser Parser::_instance;

Parser &Parser::getInstance() {
    return _instance;
}

void Parser::parseConfig(std::string &configFileName) {
    std::ifstream configFile(configFileName);

    if(!_config.empty()) {
        _config.clear();
    }

    if (!configFile.is_open()) {
        throw std::runtime_error("Config file don't open");
    }

    for(int i =0; i< _grammar.getGrammarCount(); i++){
        std::string parameter;
        std::string value;
        configFile >> parameter;
        configFile >> value;
        Grammar::ConfigParams cp = _grammar.getGrammarValue(parameter);
        if(cp == Grammar::DEFAULT) {
            throw std::runtime_error("Config file don't open");
        }
        _config.insert({cp, value});
    }
    //
    syslog(LOG_INFO, "DONE");
}

std::map<Parser::Grammar::ConfigParams, std::string> Parser::getParams() {
    return _config;
}

Parser::Grammar::ConfigParams Parser::Grammar::getGrammarValue(const std::string value) {
    Parser::Grammar::ConfigParams result = _grammar.at(value);
    return result;
}

Parser::Grammar::Grammar():
    _grammar({
        {"time", ConfigParams::TIME_DELAY},
        {"dir1", ConfigParams::DIRECTORY1},
        {"dir2", ConfigParams::DIRECTORY2} }) {};

int Parser::Grammar::getGrammarCount() {
    return _grammar.size();
}
