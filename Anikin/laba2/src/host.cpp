#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <cstring>
#include "host.h"
#include "client.h"

std::shared_ptr<lab::host> lab::host::_inst = nullptr;


lab::host::host()
{
    openlog("m_ipc", LOG_NDELAY | LOG_PID, LOG_USER);
}

std::weak_ptr<lab::host> lab::host::inst()
{
    if (_inst == nullptr)
        _inst.reset(new host());

    return _inst;
}

int lab::host::init(size_t n_clients)
{
    if (_thr_pool.size() != 0 || _pid_pool.size() != 0 || n_clients == 0){
        syslog(LOG_ERR, "host init error");
        return -1;
    }
    _thr_pool.resize(n_clients);
    _pid_pool.resize(n_clients);

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
            break;
        }
    }
    return 0;
}

int lab::host::run()
{

    return _wolf.start_game();
}