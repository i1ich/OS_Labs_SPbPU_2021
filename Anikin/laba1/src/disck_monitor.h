#include <unordered_map>
#include <fstream>
#include "worker.h"
#include "cfg_parser.h"

class watchers_table{
public:

    watchers_table();
    
    ~watchers_table();

    std::string operator[](int watchfd) const;

    int add(std::string const& path);

    int remove(int watchfd);

    int remove_all();

    int get_changes(char* buffer, size_t bf_size) const;

    private:

    int _fd;
    std::unordered_map<int, std::string> _table;

};


class disk_monitor : public worker{
public:

    disk_monitor();

    ~disk_monitor();

    void run() override;

    void read_cfg(config const& cfg) override;

private:

    watchers_table _wtable;
    std::ofstream _logfile;
};