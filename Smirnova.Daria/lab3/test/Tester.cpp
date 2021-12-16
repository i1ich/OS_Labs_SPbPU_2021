#include <random>
#include <iostream>
#include <set>

#include "Tester.h"
#include "../src/CoarseGrainedSet.h"
#include "../src/OptimisticSyncSet.h"


void Tester::run(SetType setType, TestType testType, int writersNum, int readerNum, int writingNum, int readingNum) {
    CheckWriters(setType, testType, writersNum, writingNum);
    CheckReaders(setType, testType, readerNum, readingNum);
    CheckReadersWriters(setType, testType, writersNum, writingNum, readerNum, readingNum);
}

void Tester::CheckWriters(SetType setType, TestType testType, int writersNum, int writingNum) {
    std::cout << "Writers test" << std::endl;
    long time = 0;
    std::vector<pthread_t> threads(writersNum);

    for (int j = 0; j < REPEAT_TIMES; ++j) {
        auto set = CreateSet(setType);
        auto items = GenerateData(testType, writersNum, writingNum);
        auto startTime = clock();
        for (int i = 0; i < writersNum; i++) {
            auto args = CreateWriterArgs(items, i, set);
            pthread_create(&threads[i], nullptr, asyncWrite, &args);
        }
        for (int i = 0; i < writersNum; i++) {
            pthread_join(threads[i], nullptr);
        }
        auto endTime = clock();
        time += endTime - startTime;
        for (auto& array: items) {
            if (!checkAllItems(set, array))
                std::cout << "NOT PASSED" << std::endl;
            else
                std::cout << "PASSED" << std::endl;
        }
        delete set;
    }
    std::cout << "time = " << time / (double)REPEAT_TIMES << std::endl;
}

void Tester::CheckReaders(SetType setType, TestType testType, int readersNum, int readingNum) {
    std::cout << "Readers test" << std::endl;
    long time = 0;
    std::vector<pthread_t> threads(readersNum);
    std::vector<bool> found;
    for (int j = 0; j < REPEAT_TIMES; ++j) {
        auto set = CreateSet(setType);
        auto items = GenerateData(testType, readersNum, readingNum);
        int itemsNum = 0;
        for (auto const& arr : items) {
            addToSet(set, arr);
            itemsNum += static_cast<int>(items.size());
        }
        found = std::vector<bool>(itemsNum, false);
        auto startTime = clock();
        for (int i = 0; i < readersNum; i++) {
            auto args = CreateReaderArgs(items, i, set, found);
            pthread_create(&threads[i], nullptr, asyncRead, &args);
        }
        for (int i = 0; i < readersNum; i++) {
            pthread_join(threads[i], nullptr);
        }
        auto endTime = clock();
        time += endTime - startTime;
        for (size_t i = 0; i < found.size(); ++i) {
            if (!found[i]) {
                std::cout << "NOT PASSED" << std::endl;
                break;
            }
            else if (i + 1 == found.size()) {
                if (set->isEmpty())
                    std::cout << "PASSED" << std::endl;
                else
                    std::cout << "NOT PASSED" << std::endl;
            }
        }
        delete set;
    }
    std::cout << "time = " << time / (double)REPEAT_TIMES << std::endl;
}

void Tester::CheckReadersWriters(SetType setType, TestType testType, int writersNum, int writingNum, int readersNum,
                                 int readingNum) {
    std::cout << "Total test:" << std::endl;
    if (readersNum * readingNum != writersNum * writingNum)
        return;
    std::vector<bool> found;
    std::vector<pthread_t> readers(readersNum);
    std::vector<pthread_t> writers(writersNum);
    long time = 0;
    for (int j = 0; j < REPEAT_TIMES; ++j) {
        auto set = CreateSet(setType);
        auto items = GenerateData(testType, writersNum, writingNum);
        int itemsNum = 0;
        for (auto const& arr : items) {
            addToSet(set, arr);
            itemsNum += static_cast<int>(items.size());
        }
        found = std::vector<bool>(itemsNum, false);

        auto startTime = clock();
        for (int i = 0; i < writersNum; i++) {
            auto args = CreateWriterArgs(items, i, set);
            pthread_create(&writers[i], nullptr, asyncWrite, &args);
        }
        for (int i = 0; i < writersNum; i++) {
            pthread_join(writers[i], nullptr);
        }

        reshape(items, readersNum);
        for (int i = 0; i < readersNum; i++) {
            auto args = CreateReaderArgs(items, i, set, found);
            pthread_create(&readers[i], nullptr, asyncRead, &args);
        }
        for (int i = 0; i < readersNum; i++) {
            pthread_join(readers[i], nullptr);
        }
        auto endTime = clock();
        time += endTime - startTime;
        for (size_t i = 0; i < found.size(); ++i) {
            if (!found[i]) {
                std::cout << "NOT PASSED" << std::endl;
                break;
            }
            else if (i + 1 == found.size()) {
                if (set->isEmpty())
                    std::cout << "PASSED" << std::endl;
                else
                    std::cout << "NOT PASSED" << std::endl;
            }
        }
        delete set;
    }
    std::cout << "time = " << time / (double)REPEAT_TIMES << std::endl;
}

std::vector<std::vector<int>> Tester::GenerateData(TestType testType, int arraysNum, int itemsNum) {
    std::vector<std::vector<int>> result(arraysNum);
    switch (testType) {
        case RANDOM_TEST_TYPE: {
            std::random_device rd;
            std::mt19937 rng(rd());
            std::uniform_int_distribution<int> uni(MIN_SET_VALUE, MAX_SET_VALUE);
            for (int i = 0; i < arraysNum; ++i) {
                std::vector<int> items(itemsNum);
                for (int j = 0; j < itemsNum; ++j)
                    items[j] = uni(rng);
                std::set<int> s( items.begin(), items.end() );
                items.assign( s.begin(), s.end() );
                result[i] = items;
            }
            break;
        }
        case SEQUENTIAL_TEST_TYPE: {
            for (int i = 0; i < arraysNum; ++i) {
                std::vector<int> items(itemsNum);
                for (int j = 0; j < itemsNum; ++j)
                    items[j] = i + j * arraysNum;
                result[i] = items;
            }
            break;
        }
        default:
            break;
    }
    return result;
}

AsyncSet<int> *Tester::CreateSet(Tester::SetType setType) {
    switch (setType) {
        case COARSE_SET_TYPE:
            return new CoarseGrainedSet<int>();
        case OPTIMISTIC_SET_TYPE:
            return new OptimisticSyncSet<int>();
        default:
            return nullptr;
    }
}

void *Tester::asyncWrite(void *writerArgs) {
    auto *args = (WriterArgs *)writerArgs;
    for (size_t i = 0; i < args->itemsToWrite.size(); ++i) {
        args->set->add(args->itemsToWrite[i]);
    }
    return nullptr;
}

void *Tester::asyncRead(void *readerArgs) {
    auto *args = (ReaderArgs *) readerArgs;
    for (size_t i = 0, foundPos = args->foundPos; i < args->readedItems.size(); ++i, ++foundPos) {
        if (args->set->contain(args->readedItems[i])) {
            args->found->at(foundPos) = true;
        }
        if (!args->set->remove(args->readedItems[i])) {
            throw std::runtime_error("Error in writer: can not remove item");
        }
    }
    return nullptr;
}

Tester::WriterArgs Tester::CreateWriterArgs(std::vector<std::vector<int>> const& items, int threadIdx,
                                            AsyncSet<int> *set) {
    auto args = Tester::WriterArgs();
    args.itemsToWrite = items[threadIdx];
    args.threadId = threadIdx;
    args.set = set;
    return args;
}

bool Tester::checkAllItems(AsyncSet<int> *set,  std::vector<int> const& array) {
    for (auto item : array) {
        if (!set->contain(item) || !set->remove(item))
            return false;
    }
    return true;
}

Tester::ReaderArgs
Tester::CreateReaderArgs(const std::vector<std::vector<int>> &items, int threadIdx, AsyncSet<int> *set,
                         std::vector<bool> &found) {
    auto args = Tester::ReaderArgs();
    args.readedItems = items[threadIdx];
    args.set = set;
    args.threadId = threadIdx;
    int counter = 0;
    for (int i = 0; i < threadIdx; ++i)
        counter += items[i].size();
    args.foundPos = counter;
    args.found = &found;
    return args;
}

void Tester::addToSet(AsyncSet<int> *set, const std::vector<int> &array) {
    if (!set)
        return;
    for (auto item : array)
        set->add(item);
}

void Tester::reshape(std::vector<std::vector<int>> &items, int arraysNum, int itemsNum) {
    std::vector<std::vector<int>> newArray(arraysNum);
    int elementsNum = 0;
    for (auto const& arr : items)
        elementsNum += arr.size();
    if (itemsNum != -1 && arraysNum * itemsNum != elementsNum)
        throw std::runtime_error("Error in reshape method: mismatching sizes");
    if (itemsNum == -1)
        itemsNum = elementsNum / arraysNum;
    size_t k = 0;
    size_t i = 0;
    for(int n = 0; n < arraysNum; ++n) {
        std::vector<int> arr(itemsNum);
        for (int j = 0; j < itemsNum; ++j) {
            if (items[k].size() >= i) {
                ++k;
                i = 0;
            }
            arr[j] = items[k][i];
            ++i;
        }
        newArray[n] = arr;
    }

    int n = 0;
    while (k + 1 < items.size()) {
        if (items[k].size() >= i) {
            ++k;
            i = 0;
        }
        newArray[n].push_back(items[k][i]);
        ++i;
        ++n;
    }
    while (i < items[k].size()) {
        newArray[n].push_back(items[k][i]);
        ++i;
        ++n;
    }
    items = newArray;
}
