#include "async_runtime/fundamental/async.h"

ThreadPool::ThreadPool(size_t threads) : stop(false)
{
    assert(threads > 0);
    workers.reserve(threads);
    for (size_t i = 0; i < threads; ++i)
        workers.emplace_back(
            [this] {
                for (;;)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock, [this] { return this->stop || !this->microTasks.empty() || !this->tasks.empty(); });

                        if ( // this->stop &&
                            this->microTasks.empty() &&
                            this->tasks.empty()) // always finish all task
                            return;

                        if (!this->microTasks.empty())
                        {
                            task = std::move(this->microTasks.front());
                            this->microTasks.pop();
                        }
                        else
                        {
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }
                    }
                    task();
                }
            });
}

size_t ThreadPool::threads() const { return this->workers.size(); }

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

void ThreadPool::forceClose()
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
        worker.detach();
}

Object::Ref<ThreadPool> AutoReleaseThreadPool::shared()
{
    static Object::Ref<ThreadPool> singleton = Object::create<AutoReleaseThreadPool>(1);
    return singleton;
}

const List<AsyncSnapshot<>::ConnectionState::Value>
    AsyncSnapshot<>::ConnectionState::values = {
        AsyncSnapshot<>::ConnectionState::none,
        AsyncSnapshot<>::ConnectionState::active,
        AsyncSnapshot<>::ConnectionState::done,
        AsyncSnapshot<>::ConnectionState::waiting};

String AsyncSnapshot<>::ConnectionState::toString(AsyncSnapshot<>::ConnectionState::Value value)
{
    static const String none = "AsyncSnapshot<>::ConnectionState::none";
    static const String active = "AsyncSnapshot<>::ConnectionState::active";
    static const String done = "AsyncSnapshot<>::ConnectionState::done";
    static const String waiting = "AsyncSnapshot<>::ConnectionState::waiting";
    switch (value)
    {
    case AsyncSnapshot<>::ConnectionState::none:
        return none;
    case AsyncSnapshot<>::ConnectionState::active:
        return active;
    case AsyncSnapshot<>::ConnectionState::done:
        return done;
    case AsyncSnapshot<>::ConnectionState::waiting:
        return waiting;
    default:
        assert(false && "The enum doesn't exists. ");
        break;
    }
}