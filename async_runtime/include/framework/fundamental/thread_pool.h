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

class EventQueue;

class ThreadPool : public virtual Object
{
public:
    ThreadPool(size_t);
    virtual ~ThreadPool();

    template <class F, class... Args>
    auto post(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;
    virtual bool dispose();

    friend EventQueue;

protected:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::list<std::function<void()>> tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

class EventQueue : public virtual Object
{
    virtual ~EventQueue();

    template <class F, class... Args>
    auto post(F &&f, Args &&... args) -> std::future<typename std::result_of<F(Args...)>::type>;

    virtual bool dispose();

protected:
    Object::WeakRef<ThreadPool> parent;
    std::list<std::function<void()> *> tasks;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
    : stop(false)
{
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back(
            [this] {
                for (;;)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                                             [this] { return this->stop || !this->tasks.empty(); });
                        if (this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop_front();
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

        // don't allow enqueueing after stopping the pool
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task] { (*task)(); });
    }
    condition.notify_one();
    return res;
}

// the destructor joins all threads
inline bool ThreadPool::dispose()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop)
            return;
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers)
        worker.join();
}

inline ThreadPool::~ThreadPool()
{
    this->dispose();
}

template <typename T, typename Container>
inline void removeFirst(T &element, Container &container)
{
    for (auto iter = container.begin(); iter != container.end(); iter++)
    {
        if (&element == *iter)
        {
            iter = container.erase(iter);
            break;
        }
    }
}

inline bool EventQueue::dispose()
{
    Object::Ref<ThreadPool> threadPool = this->parent.lock();
    assert(threadPool != nullptr);
    {
        std::unique_lock<std::mutex> lock(threadPool->queue_mutex);
        for (auto iter = this->tasks.begin(); iter != this->tasks.end(); iter++)
        {
            removeFirst(*iter, threadPool->tasks);
        }
    }
}

inline EventQueue::~EventQueue()
{
    this->dispose();
}