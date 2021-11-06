#pragma once
#include <memory>
#include "cfg_parser.h"


class worker{
public:

    virtual ~worker();

    virtual void run() = 0;

    virtual void read_cfg(config const& cfg) = 0;
};