#pragma once

#include "ConfigParser.h"
#include "Logger.h"

class DaemonGrammar : public Grammar {
public:
    DaemonGrammar();

};

class Daemon {
private:
    enum TOKENS {
        DIR_IN = 0,
        DIR_OUT,
        REPEAT_SIZE,
    };

private:
    static Daemon* inst;

    static std::string const pid_file;
    static bool terminate;
    static bool reread;

    Grammar grammar;
    std::string const cfg;
    Logger* logger;

    std::string dir_in;
    std::string dir_out;
    size_t repeat_time;
    std::string output_filename = "total.log";
    std::string extension = ".log";
    std::string absolute_path;
    bool first_line = true;

public:
    static Daemon* instance(std::string const& config);
    static void release();

    ~Daemon();

    int run();

private:
    explicit Daemon(std::string const& config);

    static void signal_handler(int signal_id);

    void parse_tokens(std::vector<ConfigParser::Token> const& tokens);

    pid_t executing_pid() const;
    int create_pid_file() const;
    void setup_handler() const;

    int reread_congig();

    int init();

    int work();
    char* file_content(std::string const& filename) const;

    Daemon() = delete;
    Daemon& operator=(Daemon const&) = delete;
};
