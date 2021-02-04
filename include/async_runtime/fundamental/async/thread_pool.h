#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

#include "../async.h"
#include "../disposable.h"
#include "thread.h"

// source from https://github.com/progschj/ThreadPool
class ThreadPool : public Object, public Disposable
{
public:
    ThreadPool(size_t);
    virtual ~ThreadPool();

    template <class F, class... Args>
    auto post(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    template <class F, class... Args>
    auto microTask(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

    virtual size_t threads() const;
    virtual bool isActive();

    virtual void detach();   // detach thread
    void dispose() override; // join thread

protected:
    virtual void onConstruction(size_t threads);
    virtual std::function<void()> workerBuilder(size_t);

    // need to keep track of threads so we can join them
    std::vector<Thread> workers;
    // the task queue
    std::queue<std::function<void()>> tasks;
    // the micro task queue
    std::queue<std::function<void()>> microTasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

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
        if (stop)
        {
            assert(std::cout << "Async task post after threadpool stopped. " << std::endl);
            (*task)();
            return res;
        }
        tasks.emplace([task] { (*task)(); });
    }
    condition.notify_one();
    return res;
}

// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::microTask(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop)
        {
            assert(std::cout << "Async task post after threadpool stopped. " << std::endl);
            (*task)();
            return res;
        }
        microTasks.emplace([task] { (*task)(); });
    }
    condition.notify_one();
    return res;
}

class AutoReleaseThreadPool : public ThreadPool
{
    struct makeSharedOnly
    {
        explicit makeSharedOnly(int) {}
    };

public:
    static Object::Ref<AutoReleaseThreadPool> factory(size_t threads = 1);

    AutoReleaseThreadPool(makeSharedOnly, size_t threads = 1) : ThreadPool(threads) {}
    virtual ~AutoReleaseThreadPool();
    void detach() override;

protected:
    void onConstruction(size_t threads) override;
    std::function<void()> workerBuilder(size_t) override;
};
