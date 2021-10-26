#include <iostream>
#include "cfg_parser.h"
#include "daemon.h"


#define DEFOULT_CFG_PATH "../etc/daemon.cfg"

int main(int argc, char ** argv)
{
      std::string cfg_path(argc >= 2 ? argv[1] : DEFOULT_CFG_PATH);

      if (app::daemon::init(cfg_path))
            app::daemon::inst()->run_workers();

      return 0;
}