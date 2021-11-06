#include "cfg_parser.h"
#include <fstream>
#include <algorithm>
#include <iostream>


config::config(std::string const& cfgPath, std::string const& del, std::string const& com) :
    _cfg_path(cfgPath),
    _delimiter(del),
    _comment(com)
{
}


void config::read()
{
    _data.clear();
    std::ifstream cfgFile(_cfg_path);
    if (!cfgFile.is_open())
        throw std::runtime_error("no config file " + _cfg_path);
    
    std::string line;
    std::size_t lineIndex = 0;
    while(!cfgFile.eof()){
        lineIndex++;
        std::getline(cfgFile, line);
        line.erase(std::remove_if(line.begin(), line.end(), [] (char x) { return std::isspace(x); }), line.end());
        if (line.starts_with(_comment) || line.size() == 0)
            continue;
        
        auto delIndex = line.find(_delimiter);
        auto comIndex = line.find(_comment);

        if  (delIndex == std::string::npos || 
            (comIndex != std::string::npos && delIndex > comIndex) ||
            (delIndex == 0 || delIndex == line.size() - 1))
        {
            cfgFile.close();
            throw std::runtime_error("invalid line in config " + std::to_string(lineIndex));
        }
        
        _data.insert(std::make_pair(line.substr(0, delIndex), line.substr(delIndex + 1, comIndex - delIndex - 1)));
    }
    cfgFile.close();
}

std::string config::operator[](std::string const& key) const
{
    auto value_it = _data.find(key);
    if (value_it == _data.end())
        return std::string("");
    return value_it->second;
}

std::string config::operator[](char const* key) const
{
    std::string str_key(key);
    return operator[](str_key);
}

void config::print()
{
    for (auto pair : _data)
        std::cout << pair.first << "=" << pair.second << std::endl;
}