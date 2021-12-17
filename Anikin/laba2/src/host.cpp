#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <cstring>
#include <sys/wait.h>
#include "host.h"
#include "client.h"

std::shared_ptr<lab::host> lab::host::_inst = nullptr;


lab::host::host()
{
    openlog("m_ipc", LOG_NDELAY | LOG_PID | LOG_PERROR, LOG_USER);
}

lab::host::~host()
{
    closelog(); 
}

std::weak_ptr<lab::host> lab::host::inst()
{
    if (_inst == nullptr)
        _inst.reset(new host());

    return _inst;
}

int lab::host::init(size_t n_clients)
{
    if (_pid_pool.size() != 0 || n_clients == 0){
        syslog(LOG_ERR, "host init error");
        return -1;
    }

    for (size_t id = 0; id < n_clients; id++){
        pid_t pid = -1;

        switch (pid = fork())
        {
        case -1:
            syslog(LOG_ERR, "fork error: %s", strerror(errno));
            return -1;

        case 0:
            client_main(id);
            return 1;
            
        default:
            if (_wolf.add_id(id) != 0)
                syslog(LOG_ERR, "invalide id %i", static_cast<int>(id));
            _pid_pool.push_back(pid);
            break;
        }
    }
    return 0;
}

int lab::host::run()
{
    auto rv = _wolf.start_game();

    int prv = 0;
    
    while (_pid_pool.size() != 0){
        auto pid = wait(&prv);
        if (pid == -1){
            syslog(LOG_ERR, "childe proc return -1: %s", strerror(errno));
            return -1;
        }
        else
            _pid_pool.erase(std::find(_pid_pool.begin(), _pid_pool.end(), pid));
    }

    return rv;
}