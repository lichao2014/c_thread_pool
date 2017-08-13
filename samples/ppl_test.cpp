#include <iostream>
#include <ppl.h>

#include "test.h"


class ThreadPoolPPL: public ThreadPool {
public:
    bool submit(ThreadCallback *cb) override
    {
        tg_.run([cb] {
            cb->onThread();
        });

        return true;
    }

private:
    concurrency::task_group tg_;
};


int
main()
{
    ThreadPoolTest test(std::shared_ptr<ThreadPool>(new ThreadPoolPPL), 1000000, std::clog);

    test.start(64);

    std::cin.get();

    return 0;
}
