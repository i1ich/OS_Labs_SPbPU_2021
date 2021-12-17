#include "host.h"


size_t const default_n_clients = 5;

int main(int argc, char** argv) 
{
    size_t n_clients = argc == 2 ? std::atoi(argv[1]) : default_n_clients;

    auto m_host = lab::host::inst().lock();

    if (m_host->init(n_clients) != 0)
        return 0;
    
    m_host->run();

    return 0;
}
