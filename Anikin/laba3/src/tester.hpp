#pragma once

#include <iostream>
#include <pthread.h>
#include <syslog.h>
#include <vector>
#include <set>
#include <memory>
#include "adapter.hpp"

namespace lab
{
    typedef int ITEM_T;

    typedef void* (*th_call)(void*);

    typedef std::set<ITEM_T> dataset;

    size_t const N_VALUES = 100;

    namespace test_data
    {
        const dataset data1 = {1, 2, 3, 4, 5};

        const dataset data2 = {6, 7, 8 , 9, 10};
        
        const dataset data12 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        const dataset empty_data = {};

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
            dataset input;
        };

        /*
            method create th_calls.size() threads
            in set insert input
            return SUCCESS if afte threads temination set equals output
        */
        THREAD_RC run_threads(size_t n_consumers, size_t n_producers);

        static void* th_sp_test(void*);

        static void* th_sc_test(void*);

        static void* th_spsc_test(void*);
    };

    
    /*
        barrier to n streams
        as I understand it one of the few ways to run multiple threads at the same time
    */
    void n_barrier(size_t n);

    std::vector<dataset> create_n_sets(size_t n, size_t set_size);
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

    for (auto const& item : m_args_block.input)
        if (!m_args_block.cont->insert(item))
            return reinterpret_cast<void*>(THREAD_RC::ERROR);
    
    return reinterpret_cast<void*>(THREAD_RC::SUCCESS);
}

template <typename CONT_T>
void* lab::tester<CONT_T>::th_sc_test(void* arg)
{
    auto& m_args_block = *reinterpret_cast<args_block*>(arg);
    n_barrier(m_args_block.n_threads);

    for (auto const& item : m_args_block.input)
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
        rc = run_threads(0, 1);
        break;
    }
    case TEST::SINGLE_CONSUMER:{
        result += test_data::SC_TEST;
        rc = run_threads(1, 0);
        break;
    }
    case TEST::SINGLE_PRODUCER_SINGLE_CONSUMER:{
        result += test_data::SPSC_TEST;
        rc = run_threads(1, 1);
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
typename lab::tester<CONT_T>::THREAD_RC lab::tester<CONT_T>::run_threads(size_t n_consumers, size_t n_producers)
{
    size_t n_threads = n_producers + n_consumers;
    std::vector<pthread_t> ths(n_threads);
    conteiner_adapter<CONT_T, ITEM_T> cont;
    THREAD_RC rc;
    std::vector<args_block> args_blocks(n_threads);
    

    auto n_sets = create_n_sets(n_producers + n_consumers, N_VALUES);

    
    for (size_t i = 0; i < n_threads; i++){
        auto& args = args_blocks[i];
        args.cont = &cont;
        args.input = n_sets[i];
        args.n_threads = n_threads;
        if (i < n_producers){
            pthread_create(&ths[i], NULL, tester::th_sp_test, reinterpret_cast<void*>(&args));
        }
        else{
            for  (auto item : n_sets[i])
                cont.insert(item);
            pthread_create(&ths[i], NULL, tester::th_sc_test, reinterpret_cast<void*>(&args));
        }
    }

    THREAD_RC thread_rc;
    for (auto& th : ths){
        pthread_join(th, reinterpret_cast<void**>(&thread_rc));
        if (thread_rc != THREAD_RC::SUCCESS)
            rc = THREAD_RC::ERROR;
    }

    dataset all_output;

    for (size_t i = 0; i < n_producers; i++)
        all_output.merge(n_sets[i]);

    return cont.equals(all_output) ? rc : THREAD_RC::ERROR;
}  