#include <iostream>
#include <vector>
#include "set_adapter.hpp"
#include "tester.hpp"



int main(int, char**) {

    auto tester = lab::tester<lab::set<int>>::inst().lock();

    tester->run(lab::tester<lab::set<int>>::TEST::SINGLE_PRODUCER);

    tester->run(lab::tester<lab::set<int>>::TEST::SINGLE_CONSUMER);

    tester->run(lab::tester<lab::set<int>>::TEST::SINGLE_PRODUCER_SINGLE_CONSUMER);
    
    return 0;
}
