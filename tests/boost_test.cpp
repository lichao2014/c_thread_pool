#include <iostream>

#include "boost/thread/thread_pool.hpp"
#include "test.h"



class ThreadPoolBoost : public test::ThreadPool {
public:
    bool submit(test::ThreadCallback *cb) override
    {
        pool_.submit([cb] {
            cb->onThread();
        });

        return true;
    }

private:
    boost::basic_thread_pool pool_;
};


int
main()
{
    std::unique_ptr<test::ThreadPool> tp(new ThreadPoolBoost);

    test::ThreadPoolTest test(std::move(tp), 1000000, std::clog);

    test.start(64);

    std::cin.get();

    return 0;
}
