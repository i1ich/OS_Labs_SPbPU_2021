//
// Created by oleg on 24.10.2021.
//

#include <fstream>
#include <sys/syslog.h>
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

std::string Parser::findPath(const std::string &file) {
    if(file.empty() || file[0] == '/'){
        return file;
    }
    else{
        return Daemon::getInstance()->getStartWorkingDirectory() + file;
    }
}

bool Parser::parse() {
    data.clear();
    std::ifstream file(configPath);
    std::string readStr;

    if(getline(file, readStr)){
        if(readStr.empty()){
            syslog(LOG_ERR, "ERROR: Empty log file\n");
            return false;
        }
    }

    if(readStr.find(TIME) == std::string::npos){
        syslog(LOG_ERR, "ERROR: Invalid config\n");
        return false;
    }

    time = atoi(readStr.substr(readStr.find(' ') + 1).c_str());

    while(getline(file, readStr)) {
        if(readStr.empty()){
            break;
        }
        if(readStr.find(SPACE) == std::string::npos){
            syslog(LOG_ERR, "ERROR: exception in init: \n");
            return false;
        }
        std::string path = findPath(readStr.substr(0, readStr.find(SPACE)));
        int depth = atoi(readStr.substr(readStr.find(SPACE) + 1).c_str());
        if(depth <= 0){
            syslog(LOG_ERR, "ERROR: Invalid data: \n");
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
    path = data.front();
    data.erase(data.begin());
    return true;
}
