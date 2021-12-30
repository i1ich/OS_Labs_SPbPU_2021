#include <random>
#include <iostream>
#include <set>
#include "../headers/Tests.h"

void Tests::run(SetType setType, TestType testType, int editorsCount, int readersNum, int editsCount, int readingNum)
{
    EditTest(setType, testType, editorsCount, editsCount);
    ReadTest(setType, testType, readersNum, readingNum);
    GeneralTest(setType, testType, editorsCount, editsCount, readersNum, readingNum);
}

void Tests::EditTest(SetType setType, TestType testType, int editorsCount, int editsCount)
{
    std::cout << "Edit test" << std::endl;
    long time = 0;
    std::vector<pthread_t> threads(editorsCount);

    for (int j = 0; j < _testsCount; ++j)
    {
        Set* set = CreateSet(setType);
        auto items = GenerateData(testType, editorsCount, editsCount);
        auto startTime = clock();
        std::vector<EditorArgs> argsArr(editorsCount);

        for (int i = 0; i < editorsCount; i++)
        {
            argsArr[i] = CreateEditorsArgs(items, i, set);
            pthread_create(&threads[i], nullptr, asyncEdit, &argsArr[i]);
        }

        for (int i = 0; i < editorsCount; i++)
        {
            pthread_join(threads[i], nullptr);
        }

        auto endTime = clock();
        time += endTime - startTime;
        for (auto& array: items)
        {
            if (!checkAllItems(set, array))
            {
                std::cout << "ERROR! Iter: " << j << std::endl;
                delete set;
                return;
            }
        }
        delete set;
    }
    std::cout << "SUCCESS with average time = " << time / (double)_testsCount << "ms of repetitions: " << _testsCount << std::endl;
}

void Tests::ReadTest(SetType setType, TestType testType, int readersNum, int readingNum)
{
    std::cout << "Readers test" << std::endl;
    long time = 0;
    std::vector<pthread_t> threads(readersNum);
    std::vector<bool> found;
    pthread_mutex_t mutex;

    if (pthread_mutex_init(&mutex, nullptr) != 0)
    {
        throw std::runtime_error("error in found array's mutex initializing");
    }

    for (int j = 0; j < _testsCount; ++j)
    {
        Set* set = CreateSet(setType);
        auto items = GenerateData(testType, readersNum, readingNum);
        int itemsNum = 0;

        for (auto const& arr : items)
        {
            addToSet(set, arr);
            itemsNum += static_cast<int>(arr.size());
        }

        found = std::vector<bool>(itemsNum, false);
        std::vector<ReaderArgs> argsArr(readersNum);
        auto startTime = clock();

        for (int i = 0; i < readersNum; i++)
        {
            argsArr[i] = CreateReaderArgs(items, i, set, found, mutex);
            pthread_create(&threads[i], nullptr, asyncRead, &argsArr[i]);
        }

        for (int i = 0; i < readersNum; i++)
        {
            pthread_join(threads[i], nullptr);
        }

        auto endTime = clock();
        time += endTime - startTime;

        for (size_t i = 0; i < found.size(); ++i)
        {
            if (!found[i])
            {
                std::cout << "ERROR! Iter: " << j << ". Element not found.\n";
                delete set;
                return;
            }
            else if (i + 1 == found.size())
            {
                if (!set->empty())
                {
                    std::cout << "ERROR! Iter: " << j << ". Set is not empty.\n";
                    delete set;
                    return;
                }
            }
        }
        delete set;
    }

    if (pthread_mutex_destroy(&mutex) != 0)
    {
        throw std::runtime_error("error in found array's mutex destroying");
    }

    std::cout << "SUCCESS! with average time = " << time / (double)_testsCount << "ms of repetitions: " << _testsCount << std::endl;
}

void Tests::GeneralTest(SetType setType, TestType testType, int editorsCount, int editsCount, int readersNum, int readingNum)
{
    std::cout << "Total test:" << std::endl;
    if (readersNum * readingNum != editorsCount * editsCount)
    {
        std:: cout << "Should be readersNum * readingNum  = editorsCount * editsCount" << std::endl;
        return;
    }

    std::vector<bool> found;
    std::vector<pthread_t> readers(readersNum);
    std::vector<pthread_t> writers(editorsCount);
    long time = 0;
    pthread_mutex_t mutex;

    if (pthread_mutex_init(&mutex, nullptr) != 0)
    {
        throw std::runtime_error("error in found array's mutex initializing");
    }

    for (int j = 0; j < _testsCount; ++j)
    {
        Set* set = CreateSet(setType);
        std::vector<std::vector<int>> items;

        try
        {
             items = GenerateData(testType, editorsCount, editsCount);
        }
        catch (std::runtime_error& err)
        {
            std::cout << err.what() << std::endl;
            continue;
        }

        int itemsNum = 0;
        for (auto const& arr : items)
        {
            addToSet(set, arr);
            itemsNum += static_cast<int>(arr.size());
        }

        found = std::vector<bool>(itemsNum, false);
        std::vector<ReaderArgs> readerArgsArr(readersNum);
        std::vector<EditorArgs> writerArgsArr(editorsCount);
        auto startTime = clock();

        for (int i = 0; i < editorsCount; i++)
        {
            writerArgsArr[i] = CreateEditorsArgs(items, i, set);
            pthread_create(&writers[i], nullptr, asyncEdit, &writerArgsArr[i]);
        }

        for (int i = 0; i < editorsCount; i++)
        {
            pthread_join(writers[i], nullptr);
        }

        reshape(items, readersNum);
        for (int i = 0; i < readersNum; i++)
        {
            readerArgsArr[i] = CreateReaderArgs(items, i, set, found, mutex);
            pthread_create(&readers[i], nullptr, asyncRead, &readerArgsArr[i]);
        }

        for (int i = 0; i < readersNum; i++)
        {
            pthread_join(readers[i], nullptr);
        }

        auto endTime = clock();
        time += endTime - startTime;

        for (size_t i = 0; i < found.size(); ++i)
        {
            if (!found[i])
            {
                std::cout << "ERROR! Iter: " << j << ". Element was not found\n";
                delete set;
                return;
            }
            else if (i + 1 == found.size())
            {
                if (!set->empty())
                {
                    std::cout << "ERROR! Iter: " << j << ". Set is not empty\n";
                    delete set;
                    return;
                }
            }
        }
        delete set;
    }

    if (pthread_mutex_destroy(&mutex) != 0)
    {
        throw std::runtime_error("error in found array's mutex destroying");
    }

    std::cout << "SUCCESS with average time = " << time / (double)_testsCount << "ms of repetitions: " << _testsCount << std::endl;
}

std::vector<std::vector<int>> Tests::GenerateData(TestType testType, int arraysNum, int itemsNum)
{
    std::vector<std::vector<int>> result(arraysNum);
    switch (testType)
    {
        case RANDOM:
        {
            std::set<int> items;
            std::random_device rd;
            std::mt19937 rng(rd());
            std::uniform_int_distribution<int> uni(_minItem, _maxItem);
            int elementsPermitted = 0;

            for (int i = 0; i < arraysNum; ++i)
            {
                elementsPermitted += itemsNum;
                int j = 0;
                while (static_cast<int>(items.size()) < elementsPermitted && j < _triesCount)
                {
                    items.insert(uni(rng));
                    j++;
                }

                if (static_cast<int>(items.size()) < elementsPermitted)
                {
                    throw std::runtime_error("Can not generate data");
                }
            }

            auto begin = items.begin(), end = items.begin();
            std::advance (end, itemsNum);

            for (int i = 0; i < arraysNum; ++i)
            {
                std::vector<int> arr(begin, end);
                result[i] = arr;
                begin = end;
                std::advance (end, itemsNum);
            }
            break;
        }

        case SEQUENTIAL:
        {
            for (int i = 0; i < arraysNum; ++i)
            {
                std::vector<int> items(itemsNum);
                for (int j = 0; j < itemsNum; ++j)
                {
                    items[j] = i + j * arraysNum;
                }
                result[i] = items;
            }
            break;
        }
        default:
            break;
    }
    return result;
}

Set *Tests::CreateSet(Tests::SetType setType)
{
    switch (setType)
    {
        case LAZYSET:
            return new LazySet();
        default:
            return nullptr;
    }
}

void *Tests::asyncEdit(void *editorArgs)
{
    auto *args = (EditorArgs *) editorArgs;
    for (size_t i = 0; i < args->itemsToWrite.size(); ++i)
    {
        for (int n = 0; n < _operationsNum; ++n)
        {
            if (args->set->add(args->itemsToWrite[i]))
            {
                break;
            }
        }
    }
    return nullptr;
}

void *Tests::asyncRead(void *readerArgs)
{
    auto *args = (ReaderArgs *) readerArgs;

    for (size_t i = 0; i < args->readedItems.size(); ++i)
    {
        bool isRemoved;
        for (int n = 0; n < _operationsNum; ++n)
        {
            if (args->set->contains(args->readedItems[i]))
            {
                pthread_mutex_lock(args->mutex);
                args->found->at(args->foundPos + i) = true;
                pthread_mutex_unlock(args->mutex);
                isRemoved = false;

                for (int m = 0; m < _operationsNum; ++m)
                {
                    if (args->set->remove(args->readedItems[i]))
                    {
                        isRemoved = true;
                        break;
                    }
                }

                if (!isRemoved)
                {
                    throw std::runtime_error("Error in writer: can not remove item");
                }
                break;
            }
        }

        if (!isRemoved)
        {
            throw std::runtime_error("Error in writer: item was not found");
        }
    }
    return nullptr;
}

Tests::EditorArgs Tests::CreateEditorsArgs(std::vector<std::vector<int>> const& items, int threadIdx, Set *set)
{
    auto args = Tests::EditorArgs();
    args.itemsToWrite = items[threadIdx];
    args.set = set;
    return args;
}

bool Tests::checkAllItems(Set *set,  std::vector<int> const& array)
{
    for (auto item : array)
    {
        if (!set->contains(item) || !set->remove(item))
        {
            return false;
        }
    }
    return true;
}

Tests::ReaderArgs Tests::CreateReaderArgs(const std::vector<std::vector<int>> &items, int threadIdx, Set *set, std::vector<bool> &found, pthread_mutex_t& mutex)
{
    auto args = Tests::ReaderArgs();
    args.readedItems = items[threadIdx];
    args.set = set;
    args.mutex = &mutex;
    int counter = 0;

    for (int i = 0; i < threadIdx; ++i)
    {
        counter += items[i].size();
    }

    args.foundPos = counter;
    args.found = &found;
    return args;
}

void Tests::addToSet(Set *set, const std::vector<int> &array)
{
    if (!set)
    {
        return;
    }

    for (auto item : array)
    {
        set->add(item);
    }
}

void Tests::reshape(std::vector<std::vector<int>> &items, int arraysNum, int itemsNum)
{
    std::vector<std::vector<int>> newArray(arraysNum);
    int elementsNum = 0;

    for (auto const& arr : items)
    {
        elementsNum += arr.size();
    }

    if (itemsNum == -1)
    {
        itemsNum = elementsNum / arraysNum;
    }

    size_t k = 0;
    size_t i = 0;
    for(int n = 0; n < arraysNum; ++n)
    {
        std::vector<int> arr(itemsNum);
        for (int j = 0; j < itemsNum; ++j)
        {
            if (items[k].size() <= i)
            {
                ++k;
                i = 0;
            }
            arr[j] = items[k][i];
            ++i;
        }
        newArray[n] = arr;
    }

    int n = 0;
    while (k + 1 < items.size())
    {
        if (items[k].size() >= i)
        {
            ++k;
            i = 0;
        }
        newArray[n].push_back(items[k][i]);
        ++i;
        ++n;
    }

    while (i < items[k].size())
    {
        newArray[n].push_back(items[k][i]);
        ++i;
        ++n;
    }
    items = newArray;
}
