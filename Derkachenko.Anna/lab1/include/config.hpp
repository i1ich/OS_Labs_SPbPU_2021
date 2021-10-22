#include <fstream>
#include <string>

class Config
{
private:
    std::string inputDirToken = "input", outputDirToken = "output", timeToken = "time";
    std::string inputDir, outputDir;
    size_t time;

public:
    Config() {}
    Config(const std::string &filepath)
    {
        read(filepath);
    }

    void read(const std::string &filepath)
    {
        std::ifstream f(filepath);
        std::string token, value;

        while (f >> token >> value)
        {
            if (token == inputDirToken)
                inputDir = value;
            else if (token == outputDirToken)
                outputDir = value;
            else if (token == timeToken)
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
