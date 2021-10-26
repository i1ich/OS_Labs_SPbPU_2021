#pragma once
#include <vector>
#include <memory>
#include "worker.h"


namespace app{

    class daemon{
    public:

        static daemon* inst();

        static bool init(std::string const& cfg_path);

        void run_workers();

        ~daemon();

    private:

        static void sig_handler(int signal);

        daemon(config const& cfg);

        daemon(daemon const&);

        daemon& operator = (daemon const&);

        void write_pid();

        pid_t leave_terminal();

        static daemon* _inst;

        config _config;

        std::shared_ptr<worker> _worker;

        bool _sighup;
        bool _sigterm;

    };

}