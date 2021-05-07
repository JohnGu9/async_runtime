#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

#include "../async.h"
#include "../disposable.h"
#include "../../basic/string.h"
#include "thread.h"

/**
 * @brief 
 * ThreadPool Object provide thread pool that handle task in other thread. 
 * ThreadPool must call dispose before drop the object. 
 * 
 * @example
 * 
 * 
 */
class ThreadPool : public Object, public Disposable
{
    static ref<Set<ref<String>>> _namePool;

public:
    static thread_local ref<String> thisThreadName;
    static void setThreadName(ref<String> name);

    ThreadPool(size_t threads, option<String> name = nullptr);
    virtual ~ThreadPool();

    template <class F, class... Args>
    auto post(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;
    template <class F, class... Args>
    auto microTask(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;

    virtual ref<String> childrenThreadName(size_t id);
    virtual size_t threads() const;
    virtual bool isActive();
    void dispose() override; // join thread

protected:
    virtual std::function<void()> workerBuilder(size_t);
    virtual void unregisterName();

    ref<String> _name;

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
    const ref<String> &name = _name;
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
        if (_stop && !ThreadPool::thisThreadName->startsWith(this->name))
        {
            debug_print("Async task post after threadpool stopped. ");
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
        if (_stop && !(ThreadPool::thisThreadName)->startsWith(this->name))
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

/**
 * @brief 
 * AutoReleaseThreadPool don't must dispose before drop so that convince as static object in program
 * And it will automatically finish all task before object drop after you call dispose and release all the ref of it
 * Really it will hold ref of itself and release the ref until dispose and finish all task
 * AutoReleaseThreadPool generation must come from AutoReleaseThreadPool::factory
 * 
 * @example
 * 
 * 
 */
class AutoReleaseThreadPool : public ThreadPool
{
    struct _FactoryOnly
    {
    };

public:
    static ref<AutoReleaseThreadPool> factory(size_t threads = 1, option<String> name = nullptr);
    AutoReleaseThreadPool(_FactoryOnly, size_t threads = 1, option<String> name = nullptr);
    virtual ~AutoReleaseThreadPool();
    void dispose() override;
    virtual void close();

protected:
    std::function<void()> workerBuilder(size_t) override;

    bool _join = false;
};
