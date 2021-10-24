#include <string>
#include <vector>

const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

std::vector<std::string> split(const std::string &s, const std::string & delimiter) {
    int index = 0, nextIndex;
    std::vector<std::string> strings;
    while ((nextIndex = (int)s.find(delimiter, index))!= (int)std::string::npos) {
        strings.push_back(s.substr(index, nextIndex - index));
        index = nextIndex + 1;
    }
    strings.push_back(s.substr(index, s.size()));
    return strings;
}