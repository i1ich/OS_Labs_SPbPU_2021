#ifndef __ORACLE_H_
#define __ORACLE_H_

#include <string>
#include "../common/commondf.h"

class oracle {
public:
    weather Generate(date d);

private:
    uint32_t seed;
};

#endif // !__ORACLE_H_
