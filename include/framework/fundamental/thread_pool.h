#pragma once

#include <vector>
#include <queue>
#include <list>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

#include "../object.h"

// source from https://github.com/progschj/ThreadPool
class ThreadPool : public virtual Object
{
public:
    ThreadPool(size_t);
    virtual ~ThreadPool();

    template <class F, class... Args>
    auto post(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    template <class F, class... Args>
    auto microTask(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    virtual bool isActive();
    virtual void dispose();

protected:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::list<std::function<void()>> tasks;
    // the micro task queue
    std::list<std::function<void()>> microTasks;

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
        assert(stop == false && "Don't allow posting after stopping the pool");
        tasks.emplace_back([task] { (*task)(); });
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
        assert(stop == false && "Don't allow posting after stopping the pool");
        microTasks.emplace_back([task] { (*task)(); });
    }
    condition.notify_one();
    return res;
}
