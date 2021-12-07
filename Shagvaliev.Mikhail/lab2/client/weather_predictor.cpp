#include "weather_predictor.h"

#include "../utils/logger.h"

WeatherPredictor::WeatherPredictor() {
    Logger& logger = Logger::getInstance();
    logger.open("predictor");

    signal(SIGUSR1, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
}

WeatherPredictor &WeatherPredictor::getInstance() {
    static WeatherPredictor weatherPredictor;
    return weatherPredictor;
}

WeatherPredictor::~WeatherPredictor() {
    Logger& logger = Logger::getInstance();

    if (_semHost != SEM_FAILED)
        sem_close(_semHost);

    if (_semClient != SEM_FAILED)
        sem_close(_semClient);

    kill(_hostPid, SIGUSR2);

    if (!_conn.close())
        exit(errno);

    logger.logInfo("Weather predictor has been terminated");
    logger.close();
}

bool WeatherPredictor::openConnection() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("Starting connection");

    if (!_conn.open(_hostPid, false)) {
        logger.logError("Connection not opened");
        return false;
    }

    _semHostName = "lab2_host";
    _semClientName = "lab2_client";

    if (!tryConnectToSem(&_semHost, _semHostName)) {
        logger.logError("Cannot connect host");
        return false;
    }
    if (!tryConnectToSem(&_semClient, _semClientName)) {
        logger.logError("Cannot connect client");
        sem_close(_semHost);
        return false;
    }

    logger.logInfo("Connection is set");
    kill(_hostPid, SIGUSR1);

    return true;
}

bool WeatherPredictor::initializeSeedBase() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("Initializing weather predictor seed");

    if (!wait(_semClient)) return false;

    Message initializerMessage;
    if (!_conn.read(&initializerMessage, sizeof(initializerMessage))) {
        logger.logError("cannot read conn");
        return false;
    }

    _seedBase = std::abs(initializerMessage.randomSeedBase);

    logger.logInfo("predictor seed is set");

    if (sem_post(_semHost) == -1) {
        logger.logError("Problem in sem_post(host)");
        return false;
    }
    return true;
}

void WeatherPredictor::run() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("Weather predictor started making temperature predictions");

    Message requestMessage;
    while (true) {
        if (!wait(_semClient)) return;

        if (!_conn.read(&requestMessage, sizeof(requestMessage))) {
            logger.logError("cannot read conn");
            return;
        }

        int temperature = getWeatherPrediction(requestMessage);
        logger.logInfo("My prediction on day " + std::to_string(requestMessage.day) + ";"
            + std::to_string(requestMessage.month) + ";" + std::to_string(requestMessage.year)
            + " is " + std::to_string(temperature));

        Message predictionMessage = { -1, temperature, -1, -1, -1 };

        if(!_conn.write(&predictionMessage, sizeof(predictionMessage))) {
            logger.logError("cannot write conn");
            return;
        }

        if (sem_post(_semHost) == -1) {
            logger.logError("Problem in sem_post(host)");
            return;
        }
    }
}

bool WeatherPredictor::wait(sem_t* sem) {
    Logger& logger = Logger::getInstance();
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += Definitions::WAIT_TIMEOUT;
    if (sem_timedwait(sem, &ts) == -1) {
        logger.logError("Wait timeout");
        return false;
    }
    return true;
}

int WeatherPredictor::getWeatherPrediction(Message& requestMessage) {
    srand( 2 + requestMessage.day + requestMessage.month + requestMessage.year + _seedBase);
    auto randomValue = rand();
    return randomValue % Definitions::MAX_ABS_TEMPERATURE * (randomValue % 2 ? 1 : -1);
}

void WeatherPredictor::signalHandler(int signal) {
    Logger& logger = Logger::getInstance();
    WeatherPredictor& weatherPredictor = getInstance();

    switch (signal) {
        case SIGTERM:
        case SIGINT:
            logger.logInfo("Received signal to terminate weather predictor");
            exit(EXIT_SUCCESS);
        case SIGUSR1:
            kill(weatherPredictor._hostPid, SIGTERM);
            exit(EXIT_SUCCESS);
        default:
            logger.logInfo("Unknown signal");
            break;
    }
}

bool WeatherPredictor::tryConnectToSem(sem_t **sem, const std::string& name) {
    for (int i = 0; i < Definitions::RECONNECT_TIMEOUT; i++) {
        *sem = sem_open(name.c_str(), 0);
        if (*sem != SEM_FAILED) {
            return true;
        }
        sleep(1);
    }
    return false;
}










