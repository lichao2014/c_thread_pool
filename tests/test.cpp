#include <iostream>

#include "test.h"
extern "C" {
    #include "thread_pool.h"
}


class ThreadPoolC : public test::ThreadPool {
public:
    ThreadPoolC(int work_count, int queue_size)
        :pool_(thread_pool_create(work_count, queue_size))
    {}

    ~ThreadPoolC()
    {
        thread_pool_destroy(pool_);
        pool_ = nullptr;
    }

    bool submit(test::ThreadCallback *cb) override
    {
        return 0 == thread_pool_submit(pool_, [](void *arg) {
            test::ThreadCallback *cb = static_cast<test::ThreadCallback *>(arg);
            cb->onThread();
        }, cb);
    }

private:
    thread_pool_t *pool_;
};


int 
main()
{
    std::unique_ptr<test::ThreadPool> tp(new ThreadPoolC(16, 1024));
    test::ThreadPoolTest test(std::move(tp), 1000000, std::cout);

    test.start(64);

    std::cin.get();


    return 0;
}
