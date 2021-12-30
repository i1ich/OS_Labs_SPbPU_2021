
#include "parser.h"
#include <vector>

const map<string, parser::ParserConfig> parser::cnfName =
{
        {"WaitingTime", parser::WAITINGTIME},
        {"TimeDifference", parser::DIFFERENCE},
        {"Dir1", parser::DIR1},
        {"Dir2", parser::DIR2}
};

configData parser::parseConfigFile(const string& configFile)
{
    configData configDataParse;
    configDataParse.waitingTime = -1;
    configDataParse.diffecrence = -1;
    configDataParse.dir1 = "";
    configDataParse.dir2 = "";
    ifstream file(configFile);
    string str;

    if (!file.is_open())
    {
        throw runtime_error("Config file don't open");
    }
    while (getline(file, str))
    {
        vector<string> data;
        string strConfig;
        for (char c : str)
        {
            if (!isspace(c) && c != separator)
            {
                strConfig.push_back(c);
            }
            if (c == separator)
            {
                if (!strConfig.empty())
                    data.push_back(strConfig);
                strConfig.clear();
            }
            if (isspace(c))
            {
                if (!strConfig.empty())
                    data.push_back(strConfig);
                strConfig.clear();
            }
        }
        if (!strConfig.empty())
            data.push_back(strConfig);
        strConfig.clear();

        if (cnfName.find(data[0]) != cnfName.end())
        {
            int time;
            switch (cnfName.at(data[0]))
            {
                case parser::WAITINGTIME:
                    try
                    {
                        time = stoi(data[1]);
                    }
                    catch (exception & e)
                    {
                        throw runtime_error("stoi exeption");
                    }
                    if (time < 0)
                    {
                        throw runtime_error("Time < 0");
                    }
                    configDataParse.waitingTime = time;
                    break;

                case parser::DIFFERENCE:
                    try
                    {
                        time = stoi(data[1]);
                    }
                    catch (exception & e)
                    {
                        throw runtime_error("stoi exeption");
                    }
                    if (time < 0)
                    {
                        throw runtime_error("Time < 0");
                    }
                    configDataParse.diffecrence = time;
                    break;

                case parser::DIR1:
                    configDataParse.dir1 = data[1];
                    break;

                case parser::DIR2:
                    configDataParse.dir2 = data[1];
                    break;
            }
        }
        else
        {
            throw runtime_error("Wrong config parse name");
        }
    }
    file.close();
    return configDataParse;
}