#include "async_runtime/fundamental/thread_pool.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#include <processthreadsapi.h>
#endif

/**
 * @brief ThreadPool implement
 *
 */

ref<Lock> ThreadPool::_lock = Object::create<Lock>();
ref<Set<ref<String>>> ThreadPool::_namePool = Object::create<Set<ref<String>>>();

ThreadPool::ThreadPool(size_t threads, option<String> name) : _name(""), _stop(false)
{
    lateref<String> n;
    if (name.isNotNull(n))
    {
        this->_name = n;
    }
    else
    {
        static finalref<String> prefix = "ThreadPool<";
        this->_name = prefix + size_t(this) + ">";
    }

    {
        assert(this->_name->isNotEmpty());
        option<Lock::UniqueLock> lk = _lock->uniqueLock();
        assert(ThreadPool::_namePool->find(this->_name) == ThreadPool::_namePool->end() && "ThreadPool name can't repeat");
        ThreadPool::_namePool->insert(this->_name);
    }
    _workers.reserve(threads);
    for (size_t i = 0; i < threads; ++i)
        _workers.emplace_back(ThreadPool::workerBuilder(i));
}
#ifndef NDEBUG
ThreadPool::~ThreadPool()
{
    {
        option<Lock::UniqueLock> lock = _lock->uniqueLock();
        assert(ThreadPool::_namePool->find(this->_name) == ThreadPool::_namePool->end());
    }
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        assert(this->_stop && "ThreadPool memory leak. ThreadPool release without call [dispose]");
    }
}
#else
ThreadPool::~ThreadPool()
{
}
#endif

std::function<void()> ThreadPool::workerBuilder(size_t threadId)
{
    return [this, threadId]
    {
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

                if (this->_stop && this->_microTasks.empty() && this->_tasks.empty())
                    return;

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
                else
                    continue;
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
            assert(false && "ThreadPool call [dispose] that already is disposed. ");
            return;
        }
        _stop = true;
    }
    _condition.notify_all();

    for (std::thread &worker : _workers)
        worker.join();
    _workers.clear();
    unregisterName();
}

void ThreadPool::unregisterName()
{
    option<Lock::UniqueLock> lock = _lock->uniqueLock();
    ThreadPool::_namePool->remove(this->_name);
}

/**
 * @brief AutoReleaseThreadPool implement
 *
 */

ref<AutoReleaseThreadPool> AutoReleaseThreadPool::factory(size_t threads, option<String> name)
{
    assert(threads > 0);
    ref<AutoReleaseThreadPool> instance = Object::create<AutoReleaseThreadPool>(AutoReleaseThreadPool::_FactoryOnly(), threads, name);
    instance->_workers.reserve(threads);
    for (size_t i = 0; i < threads; ++i)
        instance->_workers.emplace_back(instance->workerBuilder(i));
    return instance;
}

AutoReleaseThreadPool::AutoReleaseThreadPool(_FactoryOnly, size_t threads, option<String> name)
    : ThreadPool(0, name)
{
}

AutoReleaseThreadPool::~AutoReleaseThreadPool()
{
    _stop = true;
    _condition.notify_all();
    if (!_join)
    {
        for (std::thread &worker : _workers)
            worker.detach();
    }
}

void AutoReleaseThreadPool::dispose()
{
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        if (_stop)
        {
            assert(false && "AutoReleaseThreadPool call [dispose] or [close] that already is disposed. ");
            return;
        }
        _stop = true;
        _join = true;
    }
    _condition.notify_all();
    for (std::thread &worker : _workers)
        worker.join();
    _workers.clear();
    unregisterName();
}

std::function<void()> AutoReleaseThreadPool::workerBuilder(size_t threadId)
{
    ref<AutoReleaseThreadPool> self = self();
    return [this, self, threadId]
    { this->ThreadPool::workerBuilder(threadId)(); };
}
