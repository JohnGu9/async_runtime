#include "async_runtime/fundamental/thread_pool.h"

static finalref<String> prefix = "ThreadPool<";

ThreadPool::ThreadPool(size_t threads, option<String> name)
    : _name(name.ifNotNullElse([this]
                               { return String::connect(prefix, size_t(this), ">"); })),
      _stop(false)
{
    DEBUG_ASSERT(this->_name->isNotEmpty());
    _workers.reserve(threads);
    for (size_t i = 0; i < threads; ++i)
        _workers.emplace_back(ThreadPool::workerBuilder(i));
}
#ifndef NDEBUG
ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        DEBUG_ASSERT(this->_stop && "ThreadPool memory leak. ThreadPool release without call [dispose]");
    }
}
#else
ThreadPool::~ThreadPool()
{
}
#endif

std::function<void()> ThreadPool::workerBuilder(size_t threadId)
{
    return [this, threadId] { //
        ThreadUnit::setThreadName(this->childrenThreadName(threadId));
#ifndef NDEBUG
        const std::string debugThreadName = ThreadUnit::threadName->toStdString();
        const std::string debugThreadPoolRuntimeType = this->toString()->toStdString();
#endif

        while (true)
        {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(this->_queueMutex);
                this->_condition.wait(lock, [this]
                                      { return this->_stop || !this->_microTasks.empty() || !this->_tasks.empty(); });

                if (!this->_microTasks.empty())
                {
                    task = std::move(this->_microTasks.front());
                    this->_microTasks.pop();
                }
                else if (!this->_tasks.empty())
                {
                    task = std::move(this->_tasks.front());
                    this->_tasks.pop();
                }
                else if (this->_stop)
                {
                    return;
                }
                else
                {
                    continue;
                }
            }
            task();
        }
    };
}

ref<String> ThreadPool::childrenThreadName(size_t id) { return this->name + "[" + id + "]"; }

size_t ThreadPool::threads() const { return this->_workers.size(); }

bool ThreadPool::isActive()
{
    std::unique_lock<std::mutex> lock(_queueMutex);
    return !this->_stop;
}

/**
 * @brief ThreadPool destructor
 * joins all threads
 *
 */
void ThreadPool::dispose()
{
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        if (_stop)
        {
            return;
        }
        _stop = true;
    }
    _condition.notify_all();

    for (std::thread &worker : _workers)
        worker.join();
    _workers.clear();
}
