//
// Created by aleksei on 10.10.2021.
//

#include <sstream>
#include <filesystem>
#include <vector>
#include <cstring>
#include "ConfigParser.h"


using namespace std;

vector<string> ConfigParser::split(const std::string& src)
{
  size_t pos = 0;
  vector<string> result;
  auto str = src;
  while ((pos = str.find(delimiter)) != string::npos) {
    result.push_back(str.substr(0, pos));
    str.erase(0, pos + strlen(delimiter));
  }
  result.push_back(str);
  return result;
}

ConfigParser::Params ConfigParser::read_line(ifstream& fs, RC* rc)
{
  RC innerRC;
  RC* pRC = rc != nullptr ? rc : &innerRC;
  if (!fs.is_open())
  {
    *pRC = RC::FILE_CLOSED;
    return {};
  }

  string line;

  getline(fs, line);
  auto tokens = split(line);

  if (tokens.size() != 4)
  {
    *pRC = RC::WRONG_CONFIG_FORMAT;
    return {};
  }

  Params result;
  result.srcFolder = tokens[0];
  result.dstFolder = tokens[1];
  result.extension = tokens[2];
  result.subfolder = tokens[3];

  if (!filesystem::is_directory(result.srcFolder) || !filesystem::is_directory(result.dstFolder))
  {
    *pRC = RC::FILE_DOES_NOT_EXIST;
    return {};
  }

  *pRC = RC::SUCCESS;

  return result;
}
