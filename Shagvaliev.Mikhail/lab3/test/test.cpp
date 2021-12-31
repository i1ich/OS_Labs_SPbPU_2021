#include <random>
#include <set>
#include "test.h"


void Tests::run(SetFactory::SetType setType, TestType testType, int writersNum, int readersNum, int writingNum, int readingNum) {
    CheckWriters(setType, testType, writersNum, writingNum);
    CheckReaders(setType, testType, readersNum, readingNum);
    CheckReadersWriters(setType, testType, writersNum, writingNum, readersNum, readingNum);
}

void Tests::CheckWriters(SetFactory::SetType setType, TestType testType, int writersNum, int writingNum) {
    std::cout << "Writers test" << std::endl;
    long time = 0;
    std::vector<pthread_t> threads(writersNum);

    for (int j = 0; j < REPEAT_TIMES; ++j) {
        auto set = SetFactory::createTypedSet<int>(setType);
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
                return;
            }
        }
    }
    std::cout << "PASSED with average time = " << time / (double)REPEAT_TIMES << " of repetitions: " << REPEAT_TIMES << std::endl;
}

void Tests::CheckReaders(SetFactory::SetType setType, TestType testType, int readersNum, int readingNum) {
    std::cout << "Readers test" << std::endl;
    long time = 0;
    std::vector<pthread_t> threads(readersNum);
    std::vector<bool> found;
    pthread_mutex_t mutex;
    if (pthread_mutex_init(&mutex, nullptr) != 0)
        throw std::runtime_error("error in found array's mutex initializing");

    for (int j = 0; j < REPEAT_TIMES; ++j) {
        auto set = SetFactory::createTypedSet<int>(setType);
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
                return;
            }
            else if (i + 1 == found.size()) {
                if (!set->isEmpty()) {
                    std::cout << "NOT PASSED " << "at iteration #" << j << " set is not empty" << std::endl;
                    return;
                }
            }
        }
    }
    if (pthread_mutex_destroy(&mutex) != 0)
        throw std::runtime_error("error in found array's mutex destroying");

    std::cout << "PASSED with average time = " << time / (double)REPEAT_TIMES << " of repetitions: " << REPEAT_TIMES << std::endl;
}

void Tests::CheckReadersWriters(SetFactory::SetType setType, TestType testType, int writersNum, int writingNum, int readersNum,
                                int readingNum) {
    std::cout << "Total test:" << std::endl;
    if (readersNum * readingNum != writersNum * writingNum) {
        std:: cout << "Should be readersNum * readingNum  = writersNum * writingNum" << std::endl;
        return;
    }
    std::vector<bool> found;
    std::vector<pthread_t> readers(readersNum);
    std::vector<pthread_t> writers(writersNum);
    long time = 0;
    pthread_mutex_t mutex;
    if (pthread_mutex_init(&mutex, nullptr) != 0)
        throw std::runtime_error("error in found array's mutex initializing");

    for (int j = 0; j < REPEAT_TIMES; ++j) {
        auto set = SetFactory::createTypedSet<int>(setType);
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
                return;
            }
            else if (i + 1 == found.size()) {
                if (!set->isEmpty()) {
                    std::cout << "NOT PASSED " << "at iteration #" << j << " set is not empty" << std::endl;
                    return;
                }
            }
        }
    }
    if (pthread_mutex_destroy(&mutex) != 0)
        throw std::runtime_error("error in found array's mutex destroying");

    std::cout << "PASSED with average time = " << time / (double)REPEAT_TIMES << " of repetitions: " << REPEAT_TIMES << std::endl;
}

std::vector<std::vector<int>> Tests::GenerateData(TestType testType, int arraysNum, int itemsNum) {
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
            for (int i = 0; i < arraysNum; ++i) {
                std::advance (end, itemsNum);
                std::vector<int> arr(begin, end);
                result[i] = arr;
                begin = end;
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

void *Tests::asyncWrite(void *writerArgs) {
    auto *args = (WriterArgs *)writerArgs;
    for (size_t i = 0; i < args->itemsToWrite.size(); ++i) {
        for (int n = 0; n < OPERATION_ATTEMTS_NUM; ++n) {
            if (args->set->add(args->itemsToWrite[i])) {
                break;
            }
        }
    }
    return nullptr;
}

void *Tests::asyncRead(void *readerArgs) {
    auto *args = (ReaderArgs *) readerArgs;
    for (size_t i = 0; i < args->readedItems.size(); ++i) {
        bool isRemoved = false;
        for (int n = 0; n < OPERATION_ATTEMTS_NUM; ++n) {
            if (args->set->contains(args->readedItems[i])) {
                pthread_mutex_lock(args->mutex);
                args->found->at(args->foundPos + i) = true;
                pthread_mutex_unlock(args->mutex);
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
    }
    return nullptr;
}

Tests::WriterArgs Tests::CreateWriterArgs(std::vector<std::vector<int>> const& items, int threadIdx,
                                          pTypedSet<int> set) {
    auto args = Tests::WriterArgs();
    args.itemsToWrite = items[threadIdx];
    args.set = set;
    return args;
}

bool Tests::checkAllItems(pTypedSet<int> set,  std::vector<int> const& array) {
    for (auto item : array) {
        if (!set->contains(item) || !set->remove(item))
            return false;
    }
    return true;
}

Tests::ReaderArgs
Tests::CreateReaderArgs(const std::vector<std::vector<int>> &items, int threadIdx, pTypedSet<int> set,
                        std::vector<bool> &found, pthread_mutex_t& mutex) {
    auto args = Tests::ReaderArgs();
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

void Tests::addToSet(pTypedSet<int> set, const std::vector<int> &array) {
    if (!set)
        return;
    for (auto item : array)
        set->add(item);
}

void Tests::reshape(std::vector<std::vector<int>> &items, int arraysNum, int itemsNum) {
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