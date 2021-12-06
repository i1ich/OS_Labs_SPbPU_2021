#ifndef LAB2_VAR5_CONN_SEG_H
#define LAB2_VAR5_CONN_SEG_H

#include "Connection.h"

class SegConnection: public Connection {
public:
    void open(size_t hostPid, bool isCreator) override;
    void read(void *buf, size_t count) override;
    void write(void *buf, size_t count) override;
    void close() override;
private:
    bool m_isCreator;
    std::string m_connectionName;
    int m_segId;
    char* m_segMap;
};

#endif //LAB2_VAR5_CONN_SEG_H
