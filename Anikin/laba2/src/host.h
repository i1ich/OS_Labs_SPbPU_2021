#pragma once
#include <stddef.h>
#include <memory>
#include <vector>
#include "game.h"

namespace lab{

    class host{
    public:

        static std::weak_ptr<host> inst();

        int init(size_t n_clienst);

        int run();

        ~host();

    private:

        host();

        host(host const&) = delete;
        
        host& operator=(host const&) = delete;

        std::vector<pid_t> _pid_pool;

        lab::wolf _wolf;

        static std::shared_ptr<host> _inst;

    };
}