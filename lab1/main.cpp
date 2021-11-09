#include "daemon.h"
#include <iostream>
#include <syslog.h>

int main(int argc, char** argv) 
{
    try 
    {
        if( argc != 2 )
        {
            std::cout<<"Not enough arguments"<<std::endl;
            return -1;
        }
        if ( !daemon::startDaemonization( argv[1] ) )
            return -1;
    }
    catch (std::exception const& e)
    {
        syslog(LOG_ERR, "ERROR:: %s", e.what());
    }

    return 0;
}
