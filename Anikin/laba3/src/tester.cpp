#include "tester.hpp"

void lab::n_barrier(size_t n)
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

std::vector<lab::dataset> lab::create_n_sets(size_t n, size_t set_size)
{
    std::vector<dataset> vec(n);
    dataset val_set;

    while (val_set.size() < n * set_size)
        val_set.insert(rand());

    size_t count = 0;
    for (auto val : val_set){
        vec[count / set_size].insert(val);
        count++;
    }

    return vec;
}