#ifndef LAB2_VAR5_CLIENTINFO_H
#define LAB2_VAR5_CLIENTINFO_H

struct ClientInfo {
    int pid;
    bool is_ready;
    int deaths;

    ClientInfo(int pid) : pid(pid), is_ready(pid != 0), deaths(0) {}
};

#endif //LAB2_VAR5_CLIENTINFO_H
