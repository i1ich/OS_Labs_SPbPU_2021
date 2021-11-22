#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "conn_mmap.h"

#ifndef TYPE_CODE
static_assert(true, "IPC type is undefined");
#else
#define CONCAT(x, y) x ## y
#define CONN_CONCAT(x) CONCAT(Conn, x)
#define CONN_CLASS CONN_CONCAT(TYPE_CODE)
#endif

int main() {
    Weather w;
    std::tm time;
    std::istringstream ss("16.01.2001");
    ss >> std::get_time(&time, Weather::dateFormat);
    w.date = time;
    w.temp = 1488;
    auto cm = Singleton<CONN_CLASS>::getInstance();
    cm->write(w);
    Weather ww = cm->read();
    std::cout << std::put_time(&ww.date, Weather::dateFormat) << " temp: " << ww.temp << std::endl;
    return 0;
}