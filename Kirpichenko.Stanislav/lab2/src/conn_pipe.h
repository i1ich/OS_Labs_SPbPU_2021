#include "conn_interface.h"

class Connpipe: public Conn {
    
    public:
    Connpipe();
    void write(const Weather& info) override;
    Weather read() override;
    ~Connpipe();
};