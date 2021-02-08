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
    static Set<String> _namePool;

public:
    static thread_local String thisThreadName;

    ThreadPool(size_t threads, String name = nullptr);
    virtual ~ThreadPool();

    template <class F, class... Args>
    auto post(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;
    template <class F, class... Args>
    auto microTask(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;

    virtual size_t threads() const;
    virtual bool isActive();

    virtual void detach();   // detach thread
    void dispose() override; // join thread

    virtual String childrenThreadName(size_t id);

protected:
    virtual void onConstruction(size_t threads);
    virtual std::function<void()> workerBuilder(size_t);

    String _name;

    // need to keep track of threads so we can join them
    std::vector<Thread> _workers;
    // the task queue
    std::queue<std::function<void()>> _tasks;
    // the micro task queue
    std::queue<std::function<void()>> _microTasks;

    // synchronization
    std::mutex _queueMutex;
    std::condition_variable _condition;
    bool _stop;

public:
    const String &name = _name;
};

// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::post(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        if (_stop)
        {
            assert(std::cout << "Async task post after threadpool stopped. " << std::endl);
            (*task)();
            return res;
        }
        _tasks.emplace([task] { (*task)(); });
    }
    _condition.notify_one();
    return res;
}

// add new work item to the pool
template <class F, class... Args>
auto ThreadPool::microTask(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        if (_stop)
        {
            assert(std::cout << "Async task post after threadpool stopped. " << std::endl);
            (*task)();
            return res;
        }
        _microTasks.emplace([task] { (*task)(); });
    }
    _condition.notify_one();
    return res;
}

class AutoReleaseThreadPool : public ThreadPool
{
    struct MakeSharedOnly
    {
        explicit MakeSharedOnly(int) {}
    };

public:
    static Object::Ref<AutoReleaseThreadPool> factory(size_t threads = 1);

    AutoReleaseThreadPool(MakeSharedOnly, size_t threads = 1) : ThreadPool(threads) {}
    virtual ~AutoReleaseThreadPool();
    void dispose() override;
    void detach() override;

protected:
    void onConstruction(size_t threads) override;
    std::function<void()> workerBuilder(size_t) override;

    bool _join = false;
};
