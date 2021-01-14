#include "framework/fundamental/thread_pool.h"

ThreadPool::ThreadPool(size_t threads) : stop(false)
{
    assert(threads > 0);
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back(
            [this] {
                for (;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });

                        if (                     // this->stop &&
                            this->tasks.empty()) // always finish all task
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop_front();
                    }
                    task();
                }
            });
}

ThreadPool::~ThreadPool()
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    assert(this->stop && "ThreadPool memory leak. ThreadPool release without call [dispose]");
}

bool ThreadPool::isActive()
{
    std::unique_lock<std::mutex> lock(queue_mutex);
    return !this->stop;
}

// the destructor joins all threads
void ThreadPool::dispose()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop)
        {
            assert(false && "ThreadPool call [dispose] that already is disposed. ");
            return;
        }
        stop = true;
    }
    condition.notify_all();

    for (std::thread &worker : workers)
        worker.join();
}
