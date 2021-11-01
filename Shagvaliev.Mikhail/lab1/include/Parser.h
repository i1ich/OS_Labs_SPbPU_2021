//
// Created by Mekhail on 17.10.2021.
//
#pragma once

#include "definitions.h"


class Parser {
public:
    static Parser& getInstance();

    bool parseConfig(const std::string& configFile);

    int getTimeInterval();

    std::vector<DirectoryWithIgnFile> getDirectories();

    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;

private:
    Parser() = default;

    inline static const std::string TIME_NAME_IN_CONFIG = "time";

    enum class CONFIG_PARAMETERS{
        TIME_INTERVAL,
        DIRECTORIES
    };

    std::vector<DirectoryWithIgnFile>& getDirectoriesRef();
    void cleanParameters();

    std::map<CONFIG_PARAMETERS, std::any> parameters {
            {CONFIG_PARAMETERS::TIME_INTERVAL, 0},
            {CONFIG_PARAMETERS::DIRECTORIES, std::vector<DirectoryWithIgnFile>()}
    };
};
