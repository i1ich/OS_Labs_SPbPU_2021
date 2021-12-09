#ifndef __COMMONDF_H_
#define __COMMONDF_H_

#include <stdint.h>

struct date
{
    uint32_t day;
    uint32_t mon;
    uint32_t year;
    char string[128];
};

struct weather
{
    int32_t temperature;
    char string[128];
};

#endif // !__COMMONDF_H_
