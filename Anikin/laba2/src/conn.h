#pragma once
#include <cstddef>
#include <stdint.h>

namespace lab
{
    class fd_data;

    enum CONN_MODE{
        HOST,
        CLIENT
    };


    class connaction{
    public:

        connaction();
        
        ~connaction();

        int read(void* buf, size_t buf_size);

        int write(void* buf, size_t buf_size);

        int open(size_t id, CONN_MODE mode);

        int close();

    private:

        fd_data* _fd_data;
    };
}

