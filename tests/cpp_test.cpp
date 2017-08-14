#include <iostream>

#include "test.h"
#include "thread_pool.hpp"


class ThreadPoolCPP : public test::ThreadPool {
public:
    ThreadPoolCPP(int work_count, int queue_size)
        :pool_(ThreadPoolOptions(queue_size, work_count))
    {}

    bool submit(test::ThreadCallback *cb) override
    {
        return pool_.post([cb] { cb->onThread(); });
    }

private:
    ::ThreadPool pool_;
};


int
main()
{
    std::unique_ptr<test::ThreadPool> tp(new ThreadPoolCPP(16, 1024));

    test::ThreadPoolTest test(std::move(tp), 1000000, std::cout);

    test.start(64);

    std::cin.get();

    return 0;
}
