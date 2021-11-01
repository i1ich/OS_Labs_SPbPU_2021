#pragma once
#include "logger.hpp"
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

class Config
{
private:
    const std::string inputDirToken = "input", outputDirToken = "output",
                    timeToken = "time";
    std::string inputDir, outputDir;
    size_t time;
    Logger logger;

public:
    Config() {}
    Config(const std::string &filepath)
    {
        read(filepath);
    }

    Config(const Config &other)
    {
        inputDir = other.inputDir;
        outputDir = other.outputDir;
        time = other.time;
    }

    Config &operator=(const Config &other)
    {
        inputDir = other.inputDir;
        outputDir = other.outputDir;
        time = other.time;
        return *this;
    }

    void read(const std::string &filepath)
    {
        std::ifstream f(filepath);
        std::string token, value;

        while (f >> token >> value)
        {
            if (token == inputDirToken)
            {
                if (!fs::exists(value))
                {
                    logger.log("Input directory from config doesn't exist \nTerminating");
                    logger.closeSyslog();
                    std::exit(EXIT_FAILURE);
                }
                inputDir = value;
            }
            else
                if (token == outputDirToken)
                {
                    if (!fs::exists(value))
                    {
                        logger.log("Ouput directory from config doesn't exist \nTerminating");
                        logger.closeSyslog();
                        std::exit(EXIT_FAILURE);
                    }
                    outputDir = value;
                }
                else
                    if (token == timeToken)
                        time = std::stoul(value);
        }
    }

    std::string getInputDir() const
    {
        return inputDir;
    }
    std::string getOutputDir() const
    {
        return outputDir;
    }
    size_t getTime() const
    {
        return time; 
    }
};
