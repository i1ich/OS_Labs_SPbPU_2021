#include <syslog.h>
#include <memory>
#include <sstream>
#include <limits.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <errno.h>
#include "disck_monitor.h"


char const * const CFG_KEY = "dm_folders";
char const * const DM_LOG_PATH = "../log/disk_monitor.log";
int const BUFFER_SIZE =  sizeof(struct inotify_event) + NAME_MAX + 1;

typedef struct inotify_event dm_event;

worker::~worker()
{}

watchers_table::watchers_table()
{
    _fd = inotify_init();
    if (_fd < 0)
        syslog(LOG_INFO, "inotify_init return %i", _fd);
        
}

watchers_table::~watchers_table()
{
    remove_all();
    close(_fd);
}

int watchers_table::add(std::string const& path)
{
    int watchfd = inotify_add_watch(_fd, path.c_str(), IN_MODIFY);
    if (watchfd < 0){
        syslog(LOG_ERR, "inotify_add_watch return %i, errno %i", watchfd, errno);
        return watchfd;
    }
    _table.insert(std::pair<int, std::string>(watchfd, path));
    return 0;
}

int watchers_table::remove(int watchfd)
{
    if (_table.find(watchfd) == _table.end())
        return -1;
    int rv;
    if ((rv = inotify_rm_watch(_fd, watchfd)) < 0)
        syslog(LOG_ERR, "inotify_rm_watch return %i, errno %i", rv, errno);
    
    _table.erase(watchfd);

    return 0;
}

int watchers_table::remove_all()
{
    int size = _table.size();
    int rv; 
    for(auto& [watchfd, path] : _table){
        if ((rv = inotify_rm_watch(_fd, watchfd)) < 0)
            syslog(LOG_ERR, "inotify_rm_watch return %i, errno %i", rv, errno);
    }
    _table.clear();

    return size;
}

int watchers_table::get_changes(char* buffer, size_t bf_size) const
{ 
    return read(_fd, buffer, bf_size);
}

disk_monitor::disk_monitor():
    _logfile(DM_LOG_PATH, std::ios_base::app)
{
    if (!_logfile.is_open())
        syslog(LOG_ERR, "disk monitor log file not open");
}

std::string watchers_table::operator[](int watchfd) const
{
    auto it = _table.find(watchfd);
    if (it == _table.end())
        return std::string("");
    return it->second;
}


disk_monitor::~disk_monitor()
{
    _logfile.close();
}

void disk_monitor::run()
{
    if (!_logfile.is_open())
        throw std::runtime_error("disk monitor's log file not open");

    char buffer[BUFFER_SIZE];
    auto event_ptr = reinterpret_cast<dm_event*>(buffer);

    if (_wtable.get_changes(buffer, BUFFER_SIZE) < 0)
        return;
    if (event_ptr->len > 0 && (event_ptr->mask & IN_MODIFY)){
        _logfile << "[Modified] " << getpid() << " " << _wtable[event_ptr->wd] << "/" << event_ptr->name << std::endl;
        _logfile.flush();
    }
}

void disk_monitor::read_cfg(config const& cfg)
{
    syslog(LOG_INFO, "dm read config");

    _wtable.remove_all();

    std::string folders = cfg[CFG_KEY];
    if (folders == "")
        throw std::runtime_error("invalid disk monitor config");

    std::istringstream strstream(folders);

    std::string buffer;
    while(getline(strstream, buffer, ';')){
        if(_wtable.add(buffer) != 0)
            throw std::runtime_error("invalide add " + buffer);
        else
            syslog(LOG_INFO, "added folder %s", buffer.c_str());
    }
}
