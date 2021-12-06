#ifndef LAB2_VAR5_ICONNECTION_H
#define LAB2_VAR5_ICONNECTION_H

#include <cstring>
#include <string>

class Connection {
public:
    static Connection* createConnection();

    virtual void open(size_t hostPid, bool isCreator) = 0;
    virtual void read(void *buf, size_t count) = 0;
    virtual void write(void *buf, size_t count) = 0;
    virtual void close() = 0;

    virtual ~Connection() = default;
};

#endif //LAB2_VAR5_ICONNECTION_H
