#pragma once

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#include "../object.h"

class ThreadPool : public virtual Object
{
public:
    ThreadPool(size_t);
    virtual ~ThreadPool();

    template <class F, class... Args>
    auto post(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    virtual void dispose();

protected:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::queue<std::function<void()>> tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
    : stop(false)
{
    assert(threads > 0);
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back(
            [this] {
                auto self = Object::cast<ThreadPool>(this); // lock the reference
                if (self == nullptr)
                    return;

                for (;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(self->queue_mutex);
                        self->condition.wait(lock,
                                             [self] { return self->stop || !self->tasks.empty(); });
                        if (self->stop && self->tasks.empty())
                            return;
                        task = std::move(self->tasks.front());
                        self->tasks.pop();
                    }
                    task();
                }
            });
}

// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::post(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        assert(stop == false && "Don't allow posting after stopping the pool");
        tasks.emplace([task] { (*task)(); });
    }
    condition.notify_one();
    return res;
}

// the destructor joins all threads
inline void ThreadPool::dispose()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop)
            return;
        stop = true;
    }
    condition.notify_all();

#ifdef DEBUG
    for (std::thread &worker : workers)
        worker.join();
#endif
}

inline ThreadPool::~ThreadPool()
{
    this->dispose();
}
