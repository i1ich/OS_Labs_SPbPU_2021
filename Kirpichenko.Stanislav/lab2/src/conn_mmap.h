#include "conn_interface.h"
#include "singleton.h"

class Connmmap: public Conn, public Singleton<Connmmap> {
    void* _shared_memory = nullptr;
    protected:
    Connmmap();
    ~Connmmap();
    public:
    void write(const Weather& info) override;
    Weather read() override;
};