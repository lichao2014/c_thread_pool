#ifndef _TEST_H_INCLUDED
#define _TEST_H_INCLUDED


#include <chrono>
#include <atomic>
#include <ostream>
#include <memory>
//#include <cassert>

namespace test {
    class ThreadCallback {
    protected:
        virtual ~ThreadCallback() {}

    public:
        virtual void onThread() = 0;
    };


    class ThreadPool {
    public:
        virtual ~ThreadPool() {}

        virtual bool submit(ThreadCallback *cb) = 0;
    };


    class ThreadPoolTest : public ThreadCallback {
    public:
        using Clock = std::chrono::high_resolution_clock;

        ThreadPoolTest(std::unique_ptr<ThreadPool> tp, std::size_t limit, std::ostream& log)
            :tp_(std::move(tp))
            ,limit_(limit)
            ,log_(log)
        {}

        void start(size_t num)
        {
            update();

            for (size_t n = 0; n < num; ++n) {
                tp_->submit(this);
            }
        }

    private:
        void onThread() override
        {
            std::size_t count = count_.fetch_add(1) + 1;

            if ((count > limit_) && count_.compare_exchange_strong(count, 0)) {
                auto tm = update();
                log_ << tm.first - tm.second << std::endl;
            }

            tp_->submit(this);
        }

        std::pair<long long, long long> update()
        {
            auto now = std::chrono::duration_cast<std::chrono::microseconds>(Clock::now().time_since_epoch()).count();
            auto last = last_.exchange(now);
            return { now, last };
        }

        std::unique_ptr<ThreadPool> tp_;
        std::atomic_size_t count_ = 0;
        std::size_t limit_ = 0;
        std::atomic_llong last_;
        std::ostream& log_;
    };
}

#endif //_TEST_H_INCLUDED
