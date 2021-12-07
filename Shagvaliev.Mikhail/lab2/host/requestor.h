#ifndef LAB_2_REQUESTOR_H
#define LAB_2_REQUESTOR_H

#include "../utils/definitions.h"
#include "../conn/conn.h"
#include "../conn/message.h"

class Requestor {

public:
    ~Requestor();

    static Requestor& getInstance();

    bool readDates(const std::string& file);

    bool openConnection();

    bool setUpWeatherPredictor();

    void run();

private:
    struct Date {
        int day;
        int month;
        int year;
    };

    Requestor();
    Requestor(Requestor& w) = delete;
    Requestor& operator=(const Requestor& w) = delete;

    static void handleSignal(int sig, siginfo_t* sigInfo, void* ptr);

    bool wait(sem_t* sem);

    std::vector<Date> dates;

    Conn _conn;

    int _hostPid;
    int _clientPid;

    sem_t* _semHost;
    sem_t* _semClient;

    std::string _semHostName;
    std::string _semClientName;

    bool _isClientConnected;


};


#endif //LAB_2_REQUESTOR_H
