#include <iostream>

#include "test.h"
extern "C" {
    #include "thread_pool.h"
}


class ThreadPoolC : public ThreadPool {
public:
    ThreadPoolC(int work_count, int queue_size)
        :pool_(thread_pool_create(work_count, queue_size))
    {}

    ~ThreadPoolC()
    {
        thread_pool_destroy(pool_);
        pool_ = nullptr;
    }

    bool submit(ThreadCallback *cb) override
    {
        return 0 == thread_pool_submit(pool_, [](void *arg) {
            ThreadCallback *cb = static_cast<ThreadCallback *>(arg);
            cb->onThread();
        }, cb);
    }

private:
    thread_pool_t *pool_;
};


int 
main()
{
    std::shared_ptr<ThreadPool> tp(new ThreadPoolC(16, 2048));

    ThreadPoolTest test(tp, 1000000, std::cout);

    test.start(64);

    std::cin.get();

    tp.reset();

    return 0;
}
