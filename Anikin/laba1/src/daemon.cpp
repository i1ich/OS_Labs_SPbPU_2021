#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <syslog.h>
#include "daemon.h"
#include "disck_monitor.h"


#define PID_PATH "../etc/daemon.pid"

app::daemon* app::daemon::_inst = nullptr;

void app::daemon::sig_handler(int signal){
    switch (signal)
    {
    case SIGHUP:
        _inst->_sighup = true;
        syslog(LOG_INFO, "get SIGHUP");
        break;
    
    case SIGTERM:
        _inst->_sigterm = true;
        syslog(LOG_INFO, "get SIGTERM");
        break;
    
    default:
        syslog(LOG_ERR, "invalid signal %i", signal);
        break;
    }
}

app::daemon::daemon(config const& cfg):
    _config(cfg),
    _worker(new disk_monitor),
    _sighup(false),
    _sigterm(false)
{   
}

app::daemon::~daemon()
{
    closelog();
}

app::daemon* app::daemon::inst()
{
    return _inst;
}

bool app::daemon::init(std::string const& cfg_path)
{
    if (_inst != nullptr)
        return false;

    try{
        openlog("dm_daemon", LOG_NDELAY | LOG_PID, LOG_USER);
        
        config cfg(cfg_path);
        cfg.read();

        if (_inst == nullptr)
            _inst = new daemon(cfg);

        daemon::inst()->_worker->read_cfg(cfg);

        pid_t pid = _inst->leave_terminal();
        switch(pid){
        case -1:
            syslog(LOG_ERR, "bad fork");
            return false;
        case 0:
        {
            daemon::inst()->write_pid();
            struct sigaction int_handler;
            sigemptyset(&int_handler.sa_mask);
            int_handler.sa_handler = sig_handler;
            int_handler.sa_flags = 0;
            sigaction(SIGTERM, &int_handler, 0);
            sigaction(SIGHUP, &int_handler, 0);
            syslog(LOG_INFO, "proc %i initialized", getpid());
            return true;
        }
        default:
            return false;
        }
    }
    catch (std::runtime_error& e){
        syslog(LOG_ERR, "%s", e.what());
        return false;
    }
}

void app::daemon::run_workers()
{
    try{
        while(!_sigterm){
            _worker->run();
            if (_sighup){
                _config.read();
                _worker->read_cfg(_config);
                _sighup = false;
            }
        }
        _sigterm = true;
    }
    catch(std::runtime_error& e){
        syslog(LOG_ERR, "%s", e.what());
    }
}

pid_t app::daemon::leave_terminal()
{
    pid_t pid1, pid2;

    switch (pid1 = fork())
    {
    case -1:
        return -1;

    case 0:
    {  
        if (setsid()        == -1 ||
            (pid2 = fork()) == -1)
            return -1;
        
        return pid2;
    }
    default:
        return pid1;
    }
}

void app::daemon::write_pid()
{
    std::ifstream pid_infile(PID_PATH);

    if (!pid_infile.is_open())
        throw std::runtime_error("pid file is missing");
    
    pid_t pid = -1;
    pid_infile >> pid;
    if (pid != -1)
        kill(pid, SIGTERM);
    pid_infile.close();

    std::ofstream pid_outfile(PID_PATH);
    if (!pid_outfile.is_open())
        throw std::runtime_error("pid file is missing");

    pid_outfile << getpid();
    pid_outfile.close();
}