//
// Created by oleg on 24.10.2021.
//

#include <fstream>
#include <sys/syslog.h>
#include <iostream>
#include "../includes/Parser.h"
#include "../includes/Daemon.h"

const char SPACE = ' ';
const char* TIME = "time";

void Parser::setConfig(const std::string &fileName) {
    configPath = Daemon::getFullWorkingDirectory(fileName);
}

int Parser::getTime() {
    return time;
}

bool Parser::parse() {
    data.clear();
    std::ifstream file(configPath);
    std::string readStr;

    if(getline(file, readStr)){
        if(readStr.empty()){
            syslog(LOG_ERR, "ERROR: Empty log file\n");
            std::cout << "ERROR: Empty log file" << std::endl;
            return false;
        }
    }

    if(readStr.find(TIME) == std::string::npos){
        syslog(LOG_ERR, "ERROR: Invalid config\n");
        std::cout << "ERROR: Invalid config" << std::endl;
        return false;
    }

    time = atoi(readStr.substr(readStr.find(' ') + 1).c_str());

    while(getline(file, readStr)) {
        if(readStr.empty()){
            break;
        }
        if(readStr.find(SPACE) == std::string::npos){
            syslog(LOG_ERR, "ERROR: exception in init: \n");
            std::cout << "ERROR: exception in init:" << std::endl;
            return false;
        }
        std::string path = Daemon::getFullWorkingDirectory(readStr.substr(0, readStr.find(SPACE)));
        int depth = atoi(readStr.substr(readStr.find(SPACE) + 1).c_str());
        if(depth < 0){
            syslog(LOG_ERR, "ERROR: Invalid data: \n");
            std::cout << "ERROR: Invalid data:" << std::endl;
            return false;
        }
        data.emplace_back(path, depth);
    }
    return true;
}

bool Parser::getPath(std::pair<std::string, int> &path) {
    if(data.empty()){
        return false;
    }
    path = data[0];
    data.erase(data.begin());
    std::cout << path.first << std::endl;
    return true;
}
