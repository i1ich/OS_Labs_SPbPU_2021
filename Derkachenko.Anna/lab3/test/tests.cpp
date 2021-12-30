#include "setBase.hpp"
#include <cassert>
#include <cstddef>
#include <pthread.h>
#include <vector>

struct Args
{
    SetBase* set;
    std::vector<std::vector<size_t>>* readValuesByThread;
    size_t start, stop;
    pthread_mutex_t* mutex;
};

void* writeToSet(void* args)
{
    Args* argsStruct = (Args*)args;
    for (size_t index = argsStruct->start; index < argsStruct->stop; ++index)
        argsStruct->set->add(index);
    return args;
}

void* readFromSet(void* args)
{
    Args* argsStruct = (Args*)args;
    std::vector<size_t> readValues;

    for (size_t index = argsStruct->start; index < argsStruct->stop; ++index)
        if (argsStruct->set->remove(index))
            readValues.push_back(index);

    pthread_mutex_lock(argsStruct->mutex);
    argsStruct->readValuesByThread->push_back(readValues);
    pthread_mutex_unlock(argsStruct->mutex);

    return args;
}

void writersTest(SetBase &set, size_t threads, size_t maxTestNum)
{
    std::vector<pthread_t> writers(threads);
    Args args = Args{&set, nullptr, 0, maxTestNum, nullptr};

    for (size_t index = 0; index < threads; ++index)
        pthread_create(&writers[index], NULL, writeToSet, &args);

    for (size_t index = 0; index < threads; ++index)
        pthread_join(writers[index], NULL);

    for (size_t index = 0; index < maxTestNum; ++index)
        assert(set.contains(index));
}

void readersTest(SetBase &set, size_t threads, size_t maxTestNum)
{
    std::vector<pthread_t> readers(threads);
    std::vector<std::vector<size_t>> readValuesByThread;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    Args args = Args{&set, &readValuesByThread, 0, maxTestNum, &mutex};

    for (size_t index = 0; index < threads; ++index)
        pthread_create(&readers[index], NULL, readFromSet, &args);

    for (size_t index = 0; index < threads; ++index)
        pthread_join(readers[index], NULL);

    assert(set.size() == 0);
    for (size_t index = 0; index < maxTestNum; ++index)
    {
        bool found = false;
        for (auto readValues : readValuesByThread)
            for (auto value : readValues)
                if (value == index)
                    found = true;
        assert(found);
    }

    pthread_mutex_destroy(&mutex);
}