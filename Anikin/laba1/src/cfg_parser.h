#pragma once
#include <unordered_map>


class config{
public:

    config(std::string const& cfgPath, std::string const& del = "=", std::string const& com = "#");

    void read();

    void print();

    std::string operator [](std::string const& key) const;

    std::string operator [](char const* key) const;

private:

    std::unordered_map<std::string, std::string> _data;

    std::string _cfg_path;

    std::string _delimiter;
    
    std::string _comment;
};