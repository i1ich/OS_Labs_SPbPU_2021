#ifndef LAB_2_WEATHER_PREDICTOR_H
#define LAB_2_WEATHER_PREDICTOR_H

#include "../utils/definitions.h"
#include "../conn/conn.h"
#include "../conn/message.h"

class WeatherPredictor {

public:
    ~WeatherPredictor();

    static WeatherPredictor& getInstance();

    void setHostPid(int hostPid) { _hostPid = hostPid; };

    bool openConnection();

    bool initializeSeedBase();
    void run();

private:
    WeatherPredictor();
    WeatherPredictor(WeatherPredictor& w) = delete;
    WeatherPredictor& operator=(const WeatherPredictor& w) = delete;

    bool tryConnectToSem(sem_t** sem, const std::string& name);

    static void signalHandler(int signal);

    int getWeatherPrediction(Message& requestMessage);

    bool wait(sem_t* sem);

    int _seedBase;

    int _hostPid = -1;

    Conn* _conn = Conn::getConnection();

    sem_t* _semHost;
    sem_t* _semClient;

    std::string _semHostName;
    std::string _semClientName;
};

#endif //LAB_2_WEATHER_PREDICTOR_H
