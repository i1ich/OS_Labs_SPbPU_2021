#include <stdint.h>
#include <pthread.h>
#include <memory>
#include <syslog.h>
#include "game.h"
#include "conn.h"


namespace{

    class synchronizer{
    public:

        static std::weak_ptr<synchronizer> inst();

        void wait_exchange();

        void wait_calculations();

        void free_threads();

        size_t set_n_clients(size_t n_clients);

    private:

        synchronizer();

        synchronizer(synchronizer const&) = delete;

        synchronizer& operator= (synchronizer const&) = delete;

        pthread_mutex_t _num_vector_lock;

        pthread_cond_t _n_barier_cond;
                
        pthread_cond_t _calc_cond;

        size_t _n_clients;

        size_t _n_in_barier;

        static std::shared_ptr<synchronizer> _inst;
    };

    std::shared_ptr<synchronizer> synchronizer::_inst = nullptr;

    int const wolf_max_number = 100;

    bool hid(int wolf_n, int gotl_n, int n_gotls)
    {
        return std::abs((float)wolf_n - (float)gotl_n) <= 70.f /  (float)n_gotls;
    }

    bool resurrected(int wolf_n, int gotl_n, int n_gotls)
    {
        return std::abs((float)wolf_n - (float)gotl_n) <= 20 / (float)n_gotls;
    }
};

void* wolf_thread(void* args)
{
    if (args == nullptr)
        return reinterpret_cast<void*>(-1);
    
    auto* block = reinterpret_cast<lab::args_block*>(args);

    lab::connaction conn;
    if (conn.open(block->_id, lab::CONN_MODE::HOST) != 0)
        return reinterpret_cast<void*>(-1);

    size_t const n = 2;
    bool res[n] = {true, false};
    do{
        synchronizer::inst().lock()->wait_calculations();
        res[0] = block->_status;
        res[1] = block->_game_end;
        conn.write((void*)res, n * sizeof(bool));
        conn.read((void*)&(block->_num), sizeof(block->_num));
    }while (!block->_game_end);
    conn.close();
    return reinterpret_cast<void*>(0);
}

synchronizer::synchronizer():
    _num_vector_lock(PTHREAD_MUTEX_INITIALIZER),
    _n_barier_cond(PTHREAD_COND_INITIALIZER),
    _calc_cond(PTHREAD_COND_INITIALIZER),
    _n_clients(0),
    _n_in_barier(0)
{}

size_t synchronizer::set_n_clients(size_t n_clients)
{
    if (n_clients != 0)
        _n_clients = n_clients;
    return _n_clients;
}

std::weak_ptr<synchronizer> synchronizer::inst()
{
    if (_inst == nullptr)
        _inst.reset(new synchronizer);
    
    return _inst;
}

void synchronizer::free_threads()
{
    pthread_cond_broadcast(&_n_barier_cond);
}

void synchronizer::wait_calculations()
{
    pthread_mutex_lock(&_num_vector_lock);
    _n_in_barier++;

    if (_n_in_barier >= _n_clients && _n_clients != 0) /* _n_clients == 0 corresponds to not ready start thread station */
        pthread_cond_broadcast(&_calc_cond);
    
    
    pthread_cond_wait(&_n_barier_cond, &_num_vector_lock);
    pthread_mutex_unlock(&_num_vector_lock);
}

void synchronizer::wait_exchange()
{
    pthread_mutex_lock(&_num_vector_lock);

    if (_n_in_barier < _n_clients)
        pthread_cond_wait(&_calc_cond, &_num_vector_lock);
    
    _n_in_barier = 0;

    pthread_cond_broadcast(&_n_barier_cond);
    pthread_cond_wait(&_calc_cond, &_num_vector_lock);
    pthread_mutex_unlock(&_num_vector_lock);
}

int lab::wolf::add_id(size_t id)
{
    for (auto& args : _args_blocks)
        if (args._id == id)
            return -1;
    
    lab::args_block args;
    args._id = id;
    args._status = true;
    args._game_end = false;
    args._num = 0;
    _args_blocks.push_back(args);

    pthread_t th;
    pthread_create(&th, NULL, &wolf_thread, &(_args_blocks.back()));
    _threads.push_back(th);

    return 0;
}

int lab::wolf::start_game()
{
    if (_args_blocks.size() == 0)
        return -1;

    synchronizer::inst().lock()->set_n_clients(_args_blocks.size());

    size_t dead_step = 0; // number of step, when all goatlings dead in a row

    for(size_t i = 0; i < 100 && dead_step < 2; i++){
        synchronizer::inst().lock()->wait_exchange();

        if (game_step())
            dead_step++;
        else
            dead_step = 0;
    }
    if (dead_step >= 2)
        std::cout << "wolf won" << std::endl;
    else
        std::cout << "goatlings won" << std::endl;
    game_end();
    return 0;
}

bool lab::wolf::game_step()
{
    int num = (std::rand() % wolf_max_number) + 1;
    int n_goatls = _args_blocks.size();

    std::cout << "wolf num: " << num << std::endl;

    bool all_dead = true;

    for (auto& goatling: _args_blocks){

        std::cout << "goal_" << goatling._id << " num " << goatling._num << std::endl;

        if (goatling._status && !hid(num, goatling._num, n_goatls))
            goatling._status = false;
        else if(!goatling._status && resurrected(num, goatling._num, n_goatls))
            goatling._status = true;
        if (goatling._status)
            all_dead = false;
    }
    
    return all_dead;
}

void lab::wolf::game_end()
{
    for (auto& block : _args_blocks)
        block._game_end = true;

    synchronizer::inst().lock()->free_threads();

    for (auto th : _threads)
        pthread_join(th, NULL);
}