#ifndef LAB2_VAR5_CONN_MQ_H
#define LAB2_VAR5_CONN_MQ_H

#include "Connection.h"

class MqConnection : public Connection {
public:
    void open(size_t hostPid, bool isCreator) override;
    void read(void *buf, size_t count) override;
    void write(void *buf, size_t count) override;
    void close() override;
private:
    bool m_isCreator;
    std::string m_connectionName;
    int m_descriptor;
};

#endif //LAB2_VAR5_CONN_MQ_H
