#ifndef LAB2_CONN_H
#define LAB2_CONN_H

#include "../settings/GameSettings.h"

class Conn {
public:

    bool open(size_t hostPid, bool isCreator);

    bool read(void* buf, size_t count);

    bool write(void* buf, size_t count);

    bool close();

    enum class MSG_TYPE
    {
        TO_WOLF = 0,
        TO_GOAT
    };

    struct Msg {
        MSG_TYPE type;
        int data;
    };

private:

    bool _isCreator;

    std::string _hostName;

    int fd;

    void* shm;
};

#endif //LAB2_CONN_H
