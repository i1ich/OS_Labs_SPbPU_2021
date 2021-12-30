#include <thread>
#include <functional>
#include <iostream>
#include <vector>
#include <atomic>
#include <chrono>

#include "queue.h"

const int NUM_OF_THREADS = 6;
const int NUM_OF_WRITER_THREADS = 1;


void WritersCheck(void) // it for mcsp queue so i think it for 1 writer
{
#ifdef _DEBUG
    queue q;
    const int numOfVals = 30;
    auto fun = [&q](int id, int numVals)
    {
        for (int i = 0; i < numVals; i++)
            while (!q.Enqueue(i * NUM_OF_WRITER_THREADS + id))
                ;
    };

    std::thread threads[NUM_OF_WRITER_THREADS];

    std::cout << "WritersCheck: ";


    for (int i = 0; i < NUM_OF_WRITER_THREADS; i++)
        threads[i] = std::thread(fun, i, numOfVals);
    for (int i = 0; i < NUM_OF_WRITER_THREADS; i++)
        threads[i].join();

    bool pass = true;
    for (int i = 0; i < NUM_OF_WRITER_THREADS * numOfVals; i++)
        pass = pass && q.ContainsUnsafe(i);

    if (pass)
        std::cout << "pass\n";
    else
        std::cout << "fail\n";
#endif
}

void ReadersCheck(void)
{
#ifdef _DEBUG
    queue q;
    const int numOfVals = 5;

    std::cout << "ReadersCheck: ";

    q.TailUnsafe() = numOfVals * NUM_OF_THREADS;
    q.EmptyUnsafe() = false;
    for (int i = 0; i < q.TailUnsafe(); i++)
        q.DataUnsafe()[i] = i;

    std::vector<std::vector<int>> readed;
    readed.resize(NUM_OF_THREADS);
    for (auto& v : readed)
        v.resize(numOfVals);

    auto fun = [&q, &readed](int id, int numVals)
    {
        for (int i = 0; i < numVals; i++)
        {
            int v = q.Dequeue();
            if (v == queue::QUEUE_EMPTY)
            {
                i--;
                continue;
            }
            readed[id][i] = v;
        }
    };

    std::thread threads[NUM_OF_THREADS];
    for (int i = 0; i < NUM_OF_THREADS; i++)
        threads[i] = std::thread(fun, i, numOfVals);
    for (int i = 0; i < NUM_OF_THREADS; i++)
        threads[i].join();

    std::vector<int> readedReshape;
    readedReshape.resize(NUM_OF_THREADS * numOfVals);
    for (int i = 0; i < NUM_OF_THREADS; i++)
        for (int j = 0; j < numOfVals; j++)
            readedReshape[i * numOfVals + j] = readed[i][j];

    bool pass = true;
    for (int i = 0; i < NUM_OF_THREADS * numOfVals; i++)
        pass = pass && (std::find(readedReshape.begin(), readedReshape.end(), i) != readedReshape.end());

    pass = pass && q.Empty();

    if (pass)
        std::cout << "pass\n";
    else
        std::cout << "fail\n";
#endif
}

void ComplexTest(void)
{
#ifdef _DEBUG
    // scenario of test is: 0'th thread is writer, some rest is readers
    // writer write even values, odd values enququed before test
    // readers should read all values as in readers test

    queue q;
    const int numOfVals = 5;

    std::cout << "ComplexTest: \n";

    std::atomic<int> *readed = new std::atomic<int>[NUM_OF_THREADS * numOfVals];

    auto funRead = [&q, &readed](int id, int numVals)
    {
        for (int i = 0; i < numVals; i++)
        {
            int n = q.Dequeue();
            if (n == queue::QUEUE_EMPTY)
            {
                i--;
                continue;
            }
            readed[n].fetch_add(1);
        }
    };

    auto funWrite = [&q](int id, int numVals)
    {
        for (int i = 0; i < numVals; i++)
            while (!q.Enqueue(i * 2 + id))
                ;
    };

    int numReaders[] = { 1, 2, 3, 5 };

    for (int testNum = 1; testNum <= 4; testNum++)
    {
        std::vector<std::thread> threads(numReaders[testNum - 1] + 1);
        std::cout << "  test " << testNum << ": ";
        bool pass = true;

        for (int i = 0; i < NUM_OF_THREADS * numOfVals; i++)
            readed[i].store(0);
        // prefill queue
        q.TailUnsafe() = NUM_OF_THREADS * numOfVals / 2;
        for (int i = 0; i < q.TailUnsafe(); i++)
            q.DataUnsafe()[i] = i * 2 + 1;
        q.EmptyUnsafe() = false;
        // testing
        threads[0] = std::thread(funWrite, 0, NUM_OF_THREADS * numOfVals / 2);
        for (int i = 0; i < numReaders[testNum - 1]; i++)
            threads[i + 1] = std::thread(funRead, i + 1, NUM_OF_THREADS * numOfVals / numReaders[testNum - 1]);

        for (int i = 0; i < 1 + numReaders[testNum - 1]; i++)
        {
            threads[i].join();
        }

        //check results
        for (int i = 0; i < NUM_OF_THREADS * numOfVals; i++)
            pass = pass && (readed[i].load() == 1);

        pass = pass && q.Empty();

        if (pass)
            std::cout << "pass\n";
        else
            std::cout << "fail\n";
    }

    delete[] readed;
#endif
}

void ComplexPerfTest(void)
{
#ifdef _DEBUG
    // scenario of test is: 0'th thread is writer, some rest is readers
    // writer write even values, odd values enququed before test
    // readers should read all values as in readers test

    queue q;
    const int numOfVals = 5;

    std::cout << "ComplexPerfTest: \n";

    std::atomic<int>* readed = new std::atomic<int>[NUM_OF_THREADS * numOfVals];

    auto funRead = [&q, &readed](int id, int numVals)
    {
        for (int i = 0; i < numVals; i++)
        {
            int n = q.Dequeue();
            if (n == queue::QUEUE_EMPTY)
            {
                i--;
                continue;
            }
            readed[n].fetch_add(1);
        }
    };

    auto funWrite = [&q](int id, int numVals)
    {
        for (int i = 0; i < numVals; i++)
            while (!q.Enqueue(i * 2 + id))
                ;
    };

    int numReaders[] = { 1, 2, 3, 5 };

    for (int testNum = 1; testNum <= 4; testNum++)
    {
        int measureCount = 30;
        double ms = 0;
        for (int measure = 0; measure < measureCount; measure++)
        {
            std::vector<std::thread> threads(numReaders[testNum - 1] + 1);
            for (int i = 0; i < NUM_OF_THREADS * numOfVals; i++)
                readed[i].store(0);
            // prefill queue
            q.TailUnsafe() = NUM_OF_THREADS * numOfVals / 2;
            for (int i = 0; i < q.TailUnsafe(); i++)
                q.DataUnsafe()[i] = i * 2 + 1;
            q.EmptyUnsafe() = false;
            // testing
            auto t1 = std::chrono::high_resolution_clock::now();
            threads[0] = std::thread(funWrite, 0, NUM_OF_THREADS * numOfVals / 2);
            for (int i = 0; i < numReaders[testNum - 1]; i++)
                threads[i + 1] = std::thread(funRead, i + 1, NUM_OF_THREADS * numOfVals / numReaders[testNum - 1]);

            for (int i = 0; i < 1 + numReaders[testNum - 1]; i++)
                threads[i].join();
            auto t2 = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> msDuration = t2 - t1;
            ms += msDuration.count();
        }
        ms /= (double)measureCount;
        std::cout << "Perf measurment for 1 writer " << numReaders[testNum - 1] << " readers test:  " << ms << "ms\n";
    }

    delete[] readed;
#endif
}
