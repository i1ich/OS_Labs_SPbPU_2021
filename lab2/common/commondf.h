#ifndef __COMMONDF_H_
#define __COMMONDF_H_

#include <stdint.h>
#include <string.h>

struct date
{
    uint32_t day;
    uint32_t mon;
    uint32_t year;
    char string[128];
};

inline void SerializeDate(date d, char *buf)
{
  memcpy(buf, &d, sizeof(date));
}

inline date DeserializeDate(char *str)
{
  date rez;
  memcpy(&rez, str, sizeof(date));
  return rez;
}

struct weather
{
    int32_t temperature;
    char string[128];
};

inline void SerializeWeather(weather w, char *buf)
{
  memcpy(buf, &w, sizeof(weather));
}

inline weather DeserializeWeather(char *str)
{
  weather rez;
  memcpy(&rez, str, sizeof(weather));
  return rez;
}

#endif // !__COMMONDF_H_
