//
// Created by jakob on 07.10.2021.
//

#include "ConfigLoader.h"

#include <utility>

void ConfigLoader::load(LoaderTokens const *token_params)
{
    input_file.open(input_file_name);
    fine = input_file.good()&&(!input_file.eof());
    if(!fine)
    {
        return;
    }
    size_t pos;
    int token_index;
    std::string line;
    const std::string& delimeter = token_params->getDelim();
    std::string token;
    std::string value;

    input_file >> line;
    while (!input_file.eof())
    {
        pos = line.find(delimeter);
        if (pos == std::string::npos)
        {
            fine = false;
            return;
        }
        token = line.substr(0, pos);
        value = line.substr(pos+delimeter.length(), std::string::npos);
        token_index = token_params->getTokenId(token);
        token_params->setVal(value, token_index);
        input_file >> line;
    }
}

ConfigLoader::ConfigLoader(std::string const & file_name): input_file_name(file_name), input_file(file_name)
{
    fine = true;
}

bool ConfigLoader::valid() const
{
    return fine;
}


int LoaderTokens::getTokenId(const std::string &token) const
{
    return token_fun(token);
}

void LoaderTokens::setVal(const std::string &val, int token_code) const
{
    setter_fun(val, token_code);
}

LoaderTokens::LoaderTokens(std::string d, std::function<void(const std::string &, int)> sf,
                           std::function<int(const std::string &)> tf): delim(std::move(d)), setter_fun(std::move(sf)), token_fun(std::move(tf))
{
}

std::string const &LoaderTokens::getDelim() const
{
    return delim;
}
