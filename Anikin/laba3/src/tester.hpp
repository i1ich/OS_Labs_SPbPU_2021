#pragma once

#include <iostream>
#include <pthread.h>
#include <syslog.h>
#include <vector>
#include <memory>
#include "adapter.hpp"

namespace lab
{
    typedef int ITEM_T;

    typedef void* (*th_call)(void*);

    typedef std::vector<ITEM_T> array;

    namespace test_data
    {
        const array data1 = {1, 2, 3, 4, 5};

        const array data2 = {6, 7, 8 , 9, 10};
        
        const array data12 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        const array empty_data = {};

        const std::string 
        SC_TEST = "[SC TEST]\t", 
        SP_TEST = "[SP TEST]\t",
        SPSC_TEST = "[SPSC TEST]\t",
        PASSED = "PASSED",
        FAILED = "FAILED";
    }

    template <typename CONT_T>
    class tester
    {
    public:

        enum TEST{
            SINGLE_PRODUCER,
            SINGLE_CONSUMER,
            SINGLE_PRODUCER_SINGLE_CONSUMER
        };

        ~tester();

        void run(TEST test);

        static std::weak_ptr<tester> inst();

    private:

        tester();

        enum THREAD_RC{
            SUCCESS,
            ERROR
        };

        struct args_block
        {
            conteiner_adapter<CONT_T, ITEM_T>* cont;
            size_t n_threads;
        };

        /*
            method create th_calls.size() threads
            in set insert input
            return SUCCESS if afte threads temination set equals output
        */
        THREAD_RC run_threads(array const& input, array const& output, std::vector<th_call>const& th_calls);

        static void* th_sp_test(void*);

        static void* th_sc_test(void*);

        static void* th_spsc_test(void*);
    };

    
    /*
        barrier to n streams
        as I understand it one of the few ways to run multiple threads at the same time
    */
    void n_barrier(size_t n)
    {
        static size_t counter = 0;
        static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
        static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

        pthread_mutex_lock(&lock);

        counter++;
        if (counter < n)
            pthread_cond_wait(&cond, &lock);
        else
            pthread_cond_broadcast(&cond);
        counter--;

        pthread_mutex_unlock(&lock);
    }
}

template <typename CONT_T>
std::weak_ptr<lab::tester<CONT_T>> lab::tester<CONT_T>::inst()
{
    static std::shared_ptr<tester> m_inst = nullptr;
    if (m_inst == nullptr)
        m_inst.reset(new tester<CONT_T>);
    
    return m_inst;
}

template <typename CONT_T>
lab::tester<CONT_T>::tester(){
    openlog("lfs_tester", LOG_NDELAY | LOG_PID | LOG_PERROR, LOG_USER);
}

template <typename CONT_T>
lab::tester<CONT_T>::~tester()
{
    closelog();
}

template <typename CONT_T>
void* lab::tester<CONT_T>::th_sp_test(void* arg)
{
    auto& m_args_block = *reinterpret_cast<args_block*>(arg);
    n_barrier(m_args_block.n_threads);

    for (auto const& item : test_data::data1)
        if (!m_args_block.cont->insert(item))
            return reinterpret_cast<void*>(THREAD_RC::ERROR);
    
    return reinterpret_cast<void*>(THREAD_RC::SUCCESS);
}

template <typename CONT_T>
void* lab::tester<CONT_T>::th_sc_test(void* arg)
{
    auto& m_args_block = *reinterpret_cast<args_block*>(arg);
    n_barrier(m_args_block.n_threads);

    for (auto const& item : test_data::data2)
        if (!m_args_block.cont->remove(item))
            return reinterpret_cast<void*>(THREAD_RC::ERROR);
    
    return reinterpret_cast<void*>(THREAD_RC::SUCCESS);
}

template <typename CONT_T>
void lab::tester<CONT_T>::run(lab::tester<CONT_T>::TEST test)
{
    std::string result;
    THREAD_RC rc;

    switch(test)
    {
    case TEST::SINGLE_PRODUCER:{
        result += test_data::SP_TEST;
        rc = run_threads(test_data::empty_data, test_data::data1, {tester::th_sp_test});
        break;
    }
    case TEST::SINGLE_CONSUMER:{
        result += test_data::SC_TEST;
        rc = run_threads(test_data::data2, test_data::empty_data, {tester::th_sc_test});
        break;
    }
    case TEST::SINGLE_PRODUCER_SINGLE_CONSUMER:{
        result += test_data::SPSC_TEST;
        rc = run_threads(test_data::data2, test_data::data1, {tester::th_sc_test, tester::th_sp_test});
        break;
    }
    }
    if (rc == THREAD_RC::SUCCESS)
        result += test_data::PASSED;
    else
        result += test_data::FAILED;
    
    syslog(LOG_INFO, "%s", result.c_str());
}

template <typename CONT_T>
typename lab::tester<CONT_T>::THREAD_RC lab::tester<CONT_T>::run_threads(lab::array const& input, 
                                                        lab::array const& output, 
                                                        std::vector<th_call>const& th_calls)
{
    std::vector<pthread_t> ths(th_calls.size());
    conteiner_adapter<CONT_T, ITEM_T> cont;
    THREAD_RC rc;
    args_block args;
    args.cont = &cont;
    args.n_threads = th_calls.size();

    for (auto const& item : input)
        cont.insert(item);

    for (size_t i = 0; i < th_calls.size(); i++)
        pthread_create(&ths[i], NULL, th_calls[i], reinterpret_cast<void*>(&args));

    THREAD_RC thread_rc;

    for (auto& th : ths){
        pthread_join(th, reinterpret_cast<void**>(&thread_rc));
        if (thread_rc != THREAD_RC::SUCCESS)
            rc = THREAD_RC::ERROR;
    }

    return cont.equals(output) ? rc : THREAD_RC::ERROR;
}  