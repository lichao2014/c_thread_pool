#include <iostream>

#include "boost/thread/thread_pool.hpp"
#include "test.h"



class ThreadPoolCBoost : public ThreadPool {
public:
    bool submit(ThreadCallback *cb) override
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
    std::shared_ptr<ThreadPool> tp(new ThreadPoolCBoost);

    ThreadPoolTest test(tp, 1000000, std::clog);

    test.start(64);

    std::cin.get();

    tp.reset();

    return 0;
}
