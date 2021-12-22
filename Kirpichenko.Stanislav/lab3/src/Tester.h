#pragma once
#include <string>

class Tester {
public:
    virtual std::string execute() = 0;
    virtual ~Tester() = default;
};