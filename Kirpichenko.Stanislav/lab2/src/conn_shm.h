#include "conn_interface.h"

class Connshm: public Conn {
    
    public:
    Connshm();
    void write(const Weather& info) override;
    Weather read() override;
    ~Connshm();
};