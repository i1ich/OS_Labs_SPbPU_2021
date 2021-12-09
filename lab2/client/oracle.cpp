#include "oracle.h"

weather oracle::Generate(date d)
{
    weather rez = { 0 };
    int32_t dat = d.mon * 1000000 + d.day * 10000 + d.year + seed;
    rez.temperature = ((dat % 1488) - (1488 / 2)) / 3.0;

    if (!strcmp(d.string, "pupa"))
        sprintf(rez.string, "lupa");
    else if (!strcmp(d.string, "lupa"))
        sprintf(rez.string, "pupa");
    else
        sprintf(rez.string, "snowfall, tunderstorm, strong wind, cgsg forever");
}
