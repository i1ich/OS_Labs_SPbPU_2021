#include <syslog.h>
#include <iostream>
#include "client.h"
#include "conn.h"


int const max_alive_goatl_num = 100;
int const max_dead_goatl_num = 50;

void lab::client_main(size_t id)
{
    lab::connaction conn;
    if (conn.open(id, lab::CONN_MODE::CLIENT) != 0)
        return;
    
    size_t const n = 2;
    bool res[] = {true, false};
    int num;
    while (!res[1]){
        num = (std::rand() % (res[0] ? max_alive_goatl_num : max_dead_goatl_num)) + 1;
        conn.write(reinterpret_cast<void*>(&num), sizeof(num));
        conn.read((void*)(res), n * sizeof(bool));
    }
    conn.close();
}