#include "requestor.h"
#include "../utils/logger.h"
#include <fstream>

Requestor::Requestor() : _hostPid(getpid()), _isClientConnected(false) {
    Logger& logger = Logger::getInstance();
    logger.open("requestor");

    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_flags = SA_SIGINFO;
    sig.sa_sigaction = handleSignal;
    sigaction(SIGTERM, &sig, nullptr);
    sigaction(SIGUSR1, &sig, nullptr);
    sigaction(SIGUSR2, &sig, nullptr);
    sigaction(SIGINT, &sig, nullptr);
}

Requestor::~Requestor() {
    Logger& logger = Logger::getInstance();

    if (_isClientConnected) {
        _isClientConnected = false;
        kill(_clientPid, SIGTERM);
    }

    if (_semHost != SEM_FAILED)
        sem_unlink(_semHostName.c_str());

    if (_semClient != SEM_FAILED)
        sem_unlink(_semClientName.c_str());

    if (!_conn.close())
        exit(errno);

    logger.logInfo("Requestor has been terminated");
    logger.close();
}

Requestor& Requestor::getInstance() {
    static Requestor requestor;
    return requestor;
}

bool Requestor::readDates(const std::string& file) {
    Logger& logger = Logger::getInstance();
    logger.logInfo("Reading dates from " + file);

    std::ifstream infile(file);
    if (not infile.is_open()) {
        logger.logError("Cannot read file");
        return false;
    }

    std::string line;
    while (std::getline(infile, line)) {
        Date date;
        auto firstDelimiterPos = line.find(";");
        auto lastDelimiterPos = line.find_last_of(";");

        date.day = std::stoi(line.substr(0, firstDelimiterPos));
        date.month = std::stoi(line.substr(firstDelimiterPos + 1, lastDelimiterPos - firstDelimiterPos - 1));
        date.year = std::stoi(line.substr(lastDelimiterPos + 1));
        dates.push_back(date);
    }
    infile.close();
    return true;
}

bool Requestor::openConnection() {
    Logger& logger = Logger::getInstance();

    logger.logInfo("Start connection");
    logger.logInfo("My PID is " + std::to_string(_hostPid));

    if (!_conn.open(_hostPid, true)) {
        logger.logError("Connection has not been opened");
        return false;
    }

    _semHostName = "lab2_host";
    _semClientName = "lab2_client";

    _semHost = sem_open(_semHostName.c_str(), O_CREAT, 0666, 0);
    if (_semHost == SEM_FAILED) {
        logger.logError("SemHost cannot be opened");
        return false;
    }

    _semClient = sem_open(_semClientName.c_str(), O_CREAT, 0666, 0);
    if (_semClient == SEM_FAILED) {
        sem_unlink(_semHostName.c_str());
        logger.logError("SemClient cannot be opened");
        return false;
    }
    logger.logInfo("Connection is set");
    pause();

    return true;
}

bool Requestor::setUpWeatherPredictor() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("Preparing predictor (sending seed base)");

    srand(Definitions::HOST_RANDOM_SEED);
    Message initializerMessage = { rand(), -1, -1, -1, -1 };

    if (!_conn.write(&initializerMessage, sizeof(initializerMessage))) {
        logger.logError("cannot write conn");
        return false;
    }

    if (sem_post(_semClient) == -1) {
        logger.logError("Problem in sem_post(client)");
        return false;
    }
    if (!wait(_semHost)) return false;

    logger.logInfo("Successfully prepared predictor (sent seed base)");
    return true;
}

void Requestor::run() {
    Logger& logger = Logger::getInstance();
    logger.logInfo("Sending requests to predictor");

    for (auto& date: dates) {
        logger.logInfo("Sending info about day " + std::to_string(date.day) + ";"
            + std::to_string(date.month) + ";" + std::to_string(date.year));

        Message requestMessage = { -1, 0, date.day, date.month, date.year };

        if (!_conn.write(&requestMessage, sizeof(requestMessage))) {
            logger.logError("Cannot write conn");
            return;
        }

        if (sem_post(_semClient) == -1) {
            logger.logError("Problem in sem_post(client)");
            return;
        }
        if (!wait(_semHost)) return;

        Message predictionMessage;
        if (!_conn.read(&predictionMessage, sizeof(predictionMessage))) {
            logger.logError("cannot read conn");
            return;
        }

        std::string prediction = "on day " + std::to_string(date.day) + ";"
                + std::to_string(date.month) + ";" + std::to_string(date.year)
                + " temperature is " + std::to_string(predictionMessage.temperature);
        logger.logInfo("Prediction from client is " + prediction);
    }
}

bool Requestor::wait(sem_t* sem) {
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

void Requestor::handleSignal(int sig, siginfo_t *sigInfo, void *ptr) {
    Logger& logger = Logger::getInstance();
    Requestor& requestor = getInstance();

    switch (sig) {
        case SIGUSR1:
            if (requestor._isClientConnected) {
                logger.logInfo("Client has already been connected");
            } else {
                requestor._isClientConnected = true;
                requestor._clientPid = sigInfo->si_pid;
                logger.logInfo("Client connected: pid is " + std::to_string(requestor._clientPid));
            }
            break;
        case SIGTERM:
        case SIGINT:
            logger.logInfo("Received signal to terminate requestor");
            exit(EXIT_SUCCESS);
            break;
        case SIGUSR2:
            break;
        default:
            logger.logInfo("HOST: Unknown signal");
            break;
    }
}



