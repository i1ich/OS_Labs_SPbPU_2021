#pragma once
#include <list>
#include <memory>
#include <iostream>
#include <vector>
#include <pthread.h>
#include "conn.h"

namespace lab{

    struct args_block{
        bool _status;
        bool _game_end;
        int _num;
        size_t _id;
    };

    class wolf{
    public:

        int add_id(size_t id);

        int start_game();
    
    private:

        bool game_step();

        void game_end();

        std::list<args_block> _args_blocks;  

        std::vector<pthread_t> _threads;
    };
}