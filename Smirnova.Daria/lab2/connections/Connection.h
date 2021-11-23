#ifndef LAB2_VAR5_ICONNECTION_H
#define LAB2_VAR5_ICONNECTION_H

#include <cstring>
#include <string>

class Connection {
public:
    virtual void open(size_t hostPid, bool isCreator);
    virtual void read(void *buf, size_t count);
    virtual void write(void *buf, size_t count);
    virtual void close();

private:
    bool m_isCreator;
    std::string m_connectionName;
    int m_segId;
    int m_descriptor;
    char* m_segMap;
};

#endif //LAB2_VAR5_ICONNECTION_H
