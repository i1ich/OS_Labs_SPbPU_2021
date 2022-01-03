#include <random>
#include <iostream>
#include "../headers/test.h"
#include "../headers/setFine.h"

void Test::runTests(SetType set, TestType test, int numReaders, int numReading, int numWriters, int numWriting) {
    Test::testReaders(set, test, numReaders, numReading);
    Test::testWriters(set, test, numWriters, numWriting);
    Test::testAll(set, test, numReaders, numReading, numWriters, numWriting);
}

void Test::testReaders(SetType setType, TestType testType, int numReaders, int numReading) {
    std::vector <pthread_t> threads(numReaders);
    std::cout << "INFO: start test readers" << std::endl;
    long time = 0;

    for (int i = 0; i < NUM_REPEATS; i++) {
        Set *set = getSet(setType);
        auto data = getData(testType, numReaders, numReading);
        completionSet(set, data);

        auto start_time = clock();

        for (int j = 0; j < numReaders; j++) {
            ArgsReadTest *readStruct = createReadArgs(data, j, set);
            pthread_create(&threads[j], nullptr, testRead, readStruct);
        }

        for (int j = 0; j < numReaders; j++) {
            pthread_join(threads[j], nullptr);
        }

        auto end_time = clock();

        time += end_time - start_time;

        if (!set->isEmpty()) {
            std::cout << "ERROR: set isn't empty" << std::endl;
            return;
        }
    }

    std::cout << "INFO: average time: " << (double) time / NUM_REPEATS << std::endl;
    std::cout << "INFO: test readers complete successfully" << std::endl;
}

void Test::testWriters(SetType setType, TestType testType, int numWriters, int numWriting) {
    std::vector <pthread_t> threads(numWriters);
    std::cout << "INFO: start test writers" << std::endl;
    long time = 0;

    for (int j = 0; j < NUM_REPEATS; j++) {
        Set *set = getSet(setType);
        auto data = getData(testType, numWriters, numWriting);

        auto start_time = clock();
        for (int i = 0; i < numWriters; i++) {
            ArgsWriteTest *writeStruct = createWriteArgs(data, i, set);
            pthread_create(&threads[i], nullptr, testWrite, writeStruct);
        }
        for (int i = 0; i < numWriters; i++) {
            pthread_join(threads[i], nullptr);
        }
        auto end_time = clock();

        time += end_time - start_time;

        for (int i = 0; i < numWriters; i++) {
            if (!checkSet(set, data[i])) {
                std::cout << "ERROR: at iteration №" << i << " at repeat №" << j << std::endl;
                return;
            }
        }
    }
    std::cout << "INFO: writer tests passed successfully" << std::endl;
    std::cout << "INFO: average time: " << (double) time / NUM_REPEATS << std::endl;
}

void Test::testAll(SetType setType, TestType testType, int numReaders, int numReading, int numWriters, int numWriting) {
    std::cout << "INFO: start test writing - reading" << std::endl;
    long time = 0;

    std::cout << "INFO: num_readers * num_readings = num_writers * num_writings" << std::endl;
    std::vector <Args> possibleNums = getNumsAllTest(numReaders, numReading, numWriters, numWriting, 1, 1, 1, 1);
    possibleNums = withoutRepeat(possibleNums);

    for (int i = 0; i < NUM_REPEATS; i++) {

        auto start_time = clock();
        for (int j = 0; j < (int) possibleNums.size(); j++) {
            int readers = possibleNums[j].readers;
            int readings = possibleNums[j].readings;
            int writers = possibleNums[j].writers;
            int writings = possibleNums[j].writings;

            std::vector <pthread_t> readThreads(readers);
            std::vector <pthread_t> writeThreads(writers);

            Set *set = getSet(setType);
            auto dataWrite = getData(testType, writers, writings);
            auto dataRead = reformatData(dataWrite, readers, readings);

            for (int q = 0; q < writers; q++) {
                ArgsWriteTest *writeStruct = createWriteArgs(dataWrite, q, set);
                pthread_create(&writeThreads[q], nullptr, testWrite, writeStruct);
            }

            for (int q = 0; q < writers; q++) {
                pthread_join(writeThreads[q], nullptr);
            }

            for (int q = 0; q < readers; q++) {
                ArgsReadWriteTest *rwStruct = createReadWriteArgs(dataRead, q, set);
                pthread_create(&readThreads[q], nullptr, testReadWrite, rwStruct);
            }

            std::vector <std::vector<int>> check(readers);

            for (int q = 0; q < readers; q++) {
                void* temp = nullptr;
                pthread_join(readThreads[q], &temp);
                std::vector<int>* vec = (std::vector<int>*)((temp));
                check[q] = std::vector<int>(readings);
                for(auto num = 0; num < static_cast<int>(vec->size()); num++){
                    check[q].at(num) = vec->at(num);
                }
            }

            if (!checkReadWriteTest(check)) {
                std::cout << "ERROR: invalid result at iteration: " << i << std::endl;
                return;
            }
        }
        auto end_time = clock();

        time += end_time - start_time;
    }

    std::cout << "INFO: average time: " << (double) time / (NUM_REPEATS * static_cast<int>(possibleNums.size())) << std::endl;
    std::cout << "INFO: test read write complete successfully" << std::endl;
}

Set *Test::getSet(SetType type) {
    switch (type) {
        case SetType::FINE:
            return new SetFine();
        default:
            return nullptr;
    }
}

std::map<int, std::vector<int>> Test::getData(TestType type, int numProducer, int numItems) {
    std::map<int, std::vector<int>> result = {};
    std::random_device rd;
    std::mt19937 mersenne(rd());
    switch (type) {
        case TestType::RANDOM:
            for (int i = 0; i < numProducer; i++) {
                result[i] = std::vector<int>();
                for (int j = 0; j < numItems; j++) {
                    result[i].insert(result[i].begin() + j, static_cast<int>(mersenne()) % 100000);
                }
            }
            break;
        case TestType::SEQUENTIAL:
            for (int i = 0; i < numProducer; i++) {
                result[i] = std::vector<int>();
                for (int j = 0; j < numItems; j++) {
                    result[i].insert(result[i].begin() + j, i * numItems + j);
                }
            }
            break;
    }
    return result;
}

Test::ArgsWriteTest *Test::createWriteArgs(std::map<int, std::vector<int>> data, int index, Set *set) {
    auto args = new Test::ArgsWriteTest();
    args->set = set;
    args->writeData = new std::vector<int>(data[index].size());
    for (int i = 0; i < (int)data[index].size(); i++) {
        (*args->writeData)[i] = data[index][i];
    }
    return args;
}

Test::ArgsReadTest *Test::createReadArgs(std::map<int, std::vector<int>> data, int index, Set *set) {
    auto args = new Test::ArgsReadTest();
    args->set = set;
    args->readData = new std::vector<int>(data[index].size());
    for (int i = 0; i < (int)data[index].size(); i++) {
        (*args->readData)[i] = data[index][i];
    }
    return args;
}

Test::ArgsReadWriteTest *Test::createReadWriteArgs(std::map<int, std::vector<int>> data, int index, Set *set) {
    auto args = new Test::ArgsReadWriteTest();
    args->set = set;
    args->data = new std::vector<int>(data[index].size());
    args->check = new std::vector<int>(data[index].size());
    for (int i = 0; i < (int)data[index].size(); i++) {
        (*args->data)[i] = data[index][i];
    }
    return args;
}

void *Test::testWrite(void *args) {
    auto *data = (ArgsWriteTest *) args;
    for (int i = 0; i < (int)data->writeData->size(); i++) {
        for (int j = 0; j < NUM_ATEMPTS; j++) {
            if (data->set->add((*data->writeData)[i])) {
                break;
            }
        }
    }
    return nullptr;
}

void *Test::testRead(void *args) {
    auto *data = (ArgsReadTest *) args;
    for (int i = 0; i < (int)data->readData->size(); i++) {
        for (int j = 0; j < NUM_ATEMPTS; j++) {
            if (data->set->remove((*data->readData)[i])) {
                break;
            }
        }
    }
    return nullptr;
}

void *Test::testReadWrite(void *args) {
    auto *data = (ArgsReadWriteTest *) args;
    for (int i = 0; i < (int)data->data->size(); i++) {
        for (int j = 0; j < NUM_ATEMPTS; j++) {
            if (data->set->remove((*data->data)[i])) {
                pthread_mutex_lock(&data->thread);
                data->check->at(i)++;
                pthread_mutex_unlock(&data->thread);
                break;
            }
        }
    }
    return data->check;
}

bool Test::checkSet(Set *set, std::vector<int> &vector) {
    for (int &i: vector) {
        if (!set->contains(i)) {
            std::cout << "ERROR: not element in set" << std::endl;
            return false;
        }
    }
    return true;
}

void Test::completionSet(Set *set, std::map<int, std::vector<int>> data) {
    for (auto elem: data) {
        for (int i = 0; i < (int)elem.second.size(); i++) {
            set->add(elem.second[i]);
        }
    }
}

std::vector <Test::Args>
Test::getNumsAllTest(int numReaders, int numReadings, int numWriters, int numWritings, int readers, int readings,
                     int writers, int writings) {
    std::vector <Test::Args> result;
    if (readers * readings == writers * writings) {
        struct Args args = {readers, readings, writers, writings};
        result.push_back(args);
    }
    if (readers < numReaders) {
        std::vector <Test::Args> r = getNumsAllTest(numReaders, numReadings, numWriters, numWritings, readers + 1,
                                                    readings,
                                                    writers, writings);
        for (int i = 0; i < (int)r.size(); i++) {
            result.push_back(r[i]);
        }
    }
    if (readings < numReadings) {
        std::vector <Test::Args> r = getNumsAllTest(numReaders, numReadings, numWriters, numWritings, readers,
                                                    readings + 1,
                                                    writers, writings);
        for (int i = 0; i < (int)r.size(); i++) {
            result.push_back(r[i]);
        }
    }
    if (writers < numWriters) {
        std::vector <Test::Args> r = getNumsAllTest(numReaders, numReadings, numWriters, numWritings, readers, readings,
                                                    writers + 1, writings);
        for (int i = 0; i < (int)r.size(); i++) {
            result.push_back(r[i]);
        }
    }
    if (writings < numWritings) {
        std::vector <Test::Args> r = getNumsAllTest(numReaders, numReadings, numWriters, numWritings, readers, readings,
                                                    writers, writings + 1);
        for (int i = 0; i < (int)r.size(); i++) {
            result.push_back(r[i]);
        }
    }
    return result;
}

std::vector <Test::Args> Test::withoutRepeat(std::vector <Args> args) {
    std::vector <Test::Args> result(0);
    for (int i = 0; i < (int)args.size(); i++) {
        bool have = false;
        for (int j = 0; j < (int)result.size(); j++) {
            if (result[j].readers == args[i].readers && result[j].readings == args[i].readings &&
                result[j].writers == args[i].writers && result[j].writings == args[i].writings) {
                have = true;
                break;
            }
        }
        if (!have) {
            result.push_back(args[i]);
        }
    }
    return result;
}

std::map<int, std::vector<int>> Test::reformatData(std::map<int, std::vector<int>> data, int readers, int readings) {
    std::map<int, std::vector<int>> result{};
    std::vector<int> vector(0);
    for (int i = 0; i < (int)data.size(); i++) {
        for (int j = 0; j < (int)data[i].size(); j++) {
            vector.push_back(data[i][j]);
        }
    }
    for (int i = 0; i < readers; i++) {
        for (int j = 0; j < readings; j++) {
            result[i].push_back(vector[i * readings + j]);
        }
    }
    return result;
}

bool Test::checkReadWriteTest(std::vector <std::vector<int>> data) {
    for (int i = 0; i < (int)data.size(); i++) {
        for (int j = 0; j < (int)data[i].size(); j++) {
            if (data[i][j] != 1) {
                return false;
            }
        }
    }
    return true;
}
