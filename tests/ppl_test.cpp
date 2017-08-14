#include <iostream>
#include <ppl.h>

#include "test.h"


class ThreadPoolPPL: public test::ThreadPool {
public:
    bool submit(test::ThreadCallback *cb) override
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
    std::unique_ptr<test::ThreadPool> tp(new ThreadPoolPPL);
    test::ThreadPoolTest test(std::move(tp), 1000000, std::clog);

    test.start(64);

    std::cin.get();

    return 0;
}
