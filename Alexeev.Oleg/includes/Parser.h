//
// Created by oleg on 24.10.2021.
//

#ifndef LAB1_PARSER_H
#define LAB1_PARSER_H


#include <string>
#include <vector>

class Parser {
public:
    Parser() {}
    void setConfig(const std::string& fileName);
    bool parse();
    bool getPath(std::pair<std::string, int>& path);
    int getTime();
    ~Parser() = default;
private:
    std::vector<std::pair<std::string, int>> data;
    std::string configPath;
    int time;
};


#endif //LAB1_PARSER_H
