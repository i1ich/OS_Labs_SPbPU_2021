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
        std::vector<WriterArgs> argsArr(writersNum);
        for (int i = 0; i < writersNum; i++) {
            argsArr[i] = CreateWriterArgs(items, i, set);
            pthread_create(&threads[i], nullptr, asyncWrite, &argsArr[i]);
        }
        for (int i = 0; i < writersNum; i++) {
            pthread_join(threads[i], nullptr);
        }
        auto endTime = clock();
        time += endTime - startTime;
        for (auto& array: items) {
            if (!checkAllItems(set, array)) {
                std::cout << "NOT PASSED " << "at iteration #" << j  << std::endl;
            }
            else {
                //std::cout << "PASSED "<< std::endl;
            }
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
    pthread_mutex_t mutex;
    if (pthread_mutex_init(&mutex, nullptr) != 0)
        throw std::runtime_error("error in found array's mutex initializing");

    for (int j = 0; j < REPEAT_TIMES; ++j) {
        auto set = CreateSet(setType);
        auto items = GenerateData(testType, readersNum, readingNum);
        int itemsNum = 0;
        for (auto const& arr : items) {
            addToSet(set, arr);
            itemsNum += static_cast<int>(arr.size());
        }
        found = std::vector<bool>(itemsNum, false);
        std::vector<ReaderArgs> argsArr(readersNum);
        auto startTime = clock();
        for (int i = 0; i < readersNum; i++) {
            argsArr[i] = CreateReaderArgs(items, i, set, found, mutex);
            pthread_create(&threads[i], nullptr, asyncRead, &argsArr[i]);
        }
        for (int i = 0; i < readersNum; i++) {
            pthread_join(threads[i], nullptr);
        }
        auto endTime = clock();
        time += endTime - startTime;
        for (size_t i = 0; i < found.size(); ++i) {
            if (!found[i]) {
                std::cout << "NOT PASSED " << "at iteration #" << j << " element was not found at pos " << i << std::endl;
                break;
            }
            else if (i + 1 == found.size()) {
                if (set->isEmpty()) {
                    //std::cout << "PASSED" << std::endl;
                }
                else {
                    std::cout << "NOT PASSED " << "at iteration #" << j << " set is not empty" << std::endl;
                }
            }
        }
        delete set;
    }
    if (pthread_mutex_destroy(&mutex) != 0)
        throw std::runtime_error("error in found array's mutex destroying");

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
    pthread_mutex_t mutex;
    if (pthread_mutex_init(&mutex, nullptr) != 0)
        throw std::runtime_error("error in found array's mutex initializing");

    for (int j = 0; j < REPEAT_TIMES; ++j) {
        auto set = CreateSet(setType);
        std::vector<std::vector<int>> items;
        try {
             items = GenerateData(testType, writersNum, writingNum);
        }
        catch (std::runtime_error& err) {
            std::cout << err.what() << std::endl;
            continue;
        }
        int itemsNum = 0;
        for (auto const& arr : items) {
            addToSet(set, arr);
            itemsNum += static_cast<int>(arr.size());
        }
        found = std::vector<bool>(itemsNum, false);
        std::vector<ReaderArgs> readerArgsArr(readersNum);
        std::vector<WriterArgs> writerArgsArr(writersNum);
        auto startTime = clock();
        for (int i = 0; i < writersNum; i++) {
            writerArgsArr[i] = CreateWriterArgs(items, i, set);
            pthread_create(&writers[i], nullptr, asyncWrite, &writerArgsArr[i]);
        }
        for (int i = 0; i < writersNum; i++) {
            pthread_join(writers[i], nullptr);
        }

        reshape(items, readersNum);
        for (int i = 0; i < readersNum; i++) {
            readerArgsArr[i] = CreateReaderArgs(items, i, set, found, mutex);
            pthread_create(&readers[i], nullptr, asyncRead, &readerArgsArr[i]);
        }
        for (int i = 0; i < readersNum; i++) {
            pthread_join(readers[i], nullptr);
        }
        auto endTime = clock();
        time += endTime - startTime;
        for (size_t i = 0; i < found.size(); ++i) {
            if (!found[i]) {
                std::cout << "NOT PASSED " << "at iteration #" << j << " element was not found at pos " << i << std::endl;
                std::cout << itemsNum << " items num" << std::endl;
                break;
            }
            else if (i + 1 == found.size()) {
                if (set->isEmpty()) {
                    //std::cout << "PASSED" << std::endl;
                }
                else {
                    std::cout << "NOT PASSED " << "at iteration #" << j << " set is not empty" << std::endl;
                }
            }
        }
        delete set;
    }
    if (pthread_mutex_destroy(&mutex) != 0)
        throw std::runtime_error("error in found array's mutex destroying");

    std::cout << "time = " << time / (double)REPEAT_TIMES << std::endl;
}

std::vector<std::vector<int>> Tester::GenerateData(TestType testType, int arraysNum, int itemsNum) {
    std::vector<std::vector<int>> result(arraysNum);
    switch (testType) {
        case RANDOM_TEST_TYPE: {
            std::set<int> items;
            std::random_device rd;
            std::mt19937 rng(rd());
            std::uniform_int_distribution<int> uni(MIN_SET_VALUE, MAX_SET_VALUE);
            int elementsPermitted = 0;
            for (int i = 0; i < arraysNum; ++i) {
                elementsPermitted += itemsNum;
                int j = 0;
                while (static_cast<int>(items.size()) < elementsPermitted && j < MAX_GENERATING_ATTEMPTS) {
                    items.insert(uni(rng));
                    ++j;
                }
                if (static_cast<int>(items.size()) < elementsPermitted)
                    throw std::runtime_error("Can not generate data");
            }
            auto begin = items.begin(), end = items.begin();
            std::advance (end, itemsNum);
            for (int i = 0; i < arraysNum; ++i) {
                std::vector<int> arr(begin, end);
                result[i] = arr;
                begin = end;
                std::advance (end, itemsNum);
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
            return new OptimisticSyncSet<int>(std::numeric_limits<int>::min());
        default:
            return nullptr;
    }
}

void *Tester::asyncWrite(void *writerArgs) {
    auto *args = (WriterArgs *)writerArgs;
    for (size_t i = 0; i < args->itemsToWrite.size(); ++i) {
        for (int n = 0; n < OPERATION_ATTEMTS_NUM; ++n) {
            if (args->set->add(args->itemsToWrite[i])) {
                //std::cout << "Item " << args->itemsToWrite[i] << " added" << std::endl;
                break;
            }
        }
    }
    return nullptr;
}

void *Tester::asyncRead(void *readerArgs) {
    auto *args = (ReaderArgs *) readerArgs;
    for (size_t i = 0; i < args->readedItems.size(); ++i) {
        bool isRemoved = false;
        for (int n = 0; n < OPERATION_ATTEMTS_NUM; ++n) {
            if (args->set->contain(args->readedItems[i])) {
                pthread_mutex_lock(args->mutex);
                args->found->at(args->foundPos + i) = true;
                pthread_mutex_unlock(args->mutex);
                //std::cout << "Item " << args->readedItems[i] << " readed" << std::endl;
                isRemoved = false;
                for (int m = 0; m < OPERATION_ATTEMTS_NUM; ++m) {
                    if (args->set->remove(args->readedItems[i])) {
                        isRemoved = true;
                        break;
                    }
                }
                if (!isRemoved) {
                    throw std::runtime_error("Error in writer: can not remove item");
                }
                break;
            }
        }
        if (!isRemoved) {
            throw std::runtime_error("Error in writer: item was not found");
        }
        //std::cout << "Item " << args->readedItems[i] << " removed" << std::endl;
    }
    return nullptr;
}

Tester::WriterArgs Tester::CreateWriterArgs(std::vector<std::vector<int>> const& items, int threadIdx,
                                            AsyncSet<int> *set) {
    auto args = Tester::WriterArgs();
    args.itemsToWrite = items[threadIdx];
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
                         std::vector<bool> &found, pthread_mutex_t& mutex) {
    auto args = Tester::ReaderArgs();
    args.readedItems = items[threadIdx];
    args.set = set;
    args.mutex = &mutex;
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
            if (items[k].size() <= i) {
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
