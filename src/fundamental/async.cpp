#include "async_runtime/fundamental/async.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#include <processthreadsapi.h>
#endif

/**
 * @brief ThreadPool implement
 * 
 */

static ref<String> emptyString = "";

ref<Lock> ThreadPool::_lock = Object::create<Lock>();
ref<Set<ref<String>>> ThreadPool::_namePool = Object::create<Set<ref<String>>>();

thread_local ref<String> ThreadPool::thisThreadName = "MainThread";

void ThreadPool::setThreadName(ref<String> name)
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    wchar_t *wtext = new wchar_t[name->length() + 1];
    size_t outputSize;
    mbstowcs_s(&outputSize, wtext, name->length() + 1, name->c_str(), name->length());
    SetThreadDescription(GetCurrentThread(), wtext);
    delete[] wtext;
#elif __APPLE__
    pthread_setname_np(name->c_str());
#elif __linux__
    pthread_setname_np(pthread_self(), name->c_str());
#endif
}

ThreadPool::ThreadPool(size_t threads, option<String> name) : _name(emptyString), _stop(false)
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
        ThreadPool::thisThreadName = this->childrenThreadName(threadId);
        assert(ThreadPool::thisThreadName->isNotEmpty());

#ifndef NDEBUG
        const std::string debugThreadName = ThreadPool::thisThreadName->toStdString();
        const std::string debugThreadPoolRuntimeType = this->toString()->toStdString();
        setThreadName(ThreadPool::thisThreadName);
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

/**
 * @brief Future<std::nullptr_t> implement
 * 
 */

void Future<std::nullptr_t>::sync()
{
    std::mutex mutex;
    std::condition_variable condition;
    std::unique_lock<std::mutex> lock(mutex);
    this->than([&]
               {
                   {
                       std::unique_lock<std::mutex> lock(mutex);
                   }
                   condition.notify_all();
               });
    condition.wait(lock);
}

void Future<std::nullptr_t>::sync(Duration timeout)
{
    std::mutex mutex;
    std::condition_variable condition;
    std::unique_lock<std::mutex> lock(mutex);
    this->than([&]
               {
                   {
                       std::unique_lock<std::mutex> lock(mutex);
                   }
                   condition.notify_all();
               });
    condition.wait_for(lock, timeout.toChronoMilliseconds());
}

/**
 * @brief Future<void> implement
 * 
 */

ref<Future<void>> Future<void>::race(ref<State<StatefulWidget>> state, ref<Set<ref<Future<>>>> set)
{
    if (set->empty())
        return Future<void>::value(state);
    ref<Completer<void>> completer = Object::create<Completer<void>>(getHandlerfromState(state));
    for (auto &future : set)
        future->than([completer]
                     {
                         if (completer->isCompleted == false)
                             completer->complete();
                     });
    return completer->future;
}

ref<Future<void>> Future<void>::wait(ref<State<StatefulWidget>> state, ref<Set<ref<Future<>>>> set)
{
    size_t size = set->size();
    if (size == 0)
        return Future<void>::value(state);
    ref<Completer<void>> completer = Object::create<Completer<void>>(getHandlerfromState(state));
    size_t *count = new size_t(0);
    for (auto &future : set)
        future->than([completer, count, size]
                     {
                         completer->_callbackHandler->post([completer, count, size]
                                                           {
                                                               (*count)++;
                                                               if (*count == size)
                                                               {
                                                                   completer->completeSync();
                                                                   delete count;
                                                               }
                                                           });
                     });
    return completer->future;
}

ref<Future<void>> Future<void>::value(ref<ThreadPool> callbackHandler)
{
    ref<Completer<void>> completer = Object::create<Completer<void>>(callbackHandler);
    completer->complete();
    return completer->future;
}

ref<Future<void>> Future<void>::value(ref<State<StatefulWidget>> state)
{
    ref<Completer<void>> completer = Object::create<Completer<void>>(state);
    completer->complete();
    return completer->future;
}

#include "async_runtime/fundamental/timer.h"
ref<Future<void>> Future<void>::delay(ref<ThreadPool> callbackHandler, Duration duration, option<Fn<void()>> onTimeout)
{
    ref<Completer<void>> completer = Object::create<Completer<void>>(callbackHandler);
    ref<Timer> timer = Timer::delay(callbackHandler, duration, [completer, onTimeout]
                                    {
                                        lateref<Fn<void()>> _onTimeout;
                                        if (onTimeout.isNotNull(_onTimeout))
                                            _onTimeout();
                                        completer->complete();
                                    });
    return completer->future;
}

ref<Future<void>> Future<void>::delay(ref<State<StatefulWidget>> state, Duration duration, option<Fn<void()>> onTimeout)
{
    ref<Completer<void>> completer = Object::create<Completer<void>>(state);
    ref<Timer> timer = Timer::delay(state, duration, [completer, onTimeout]
                                    {
                                        lateref<Fn<void()>> _onTimeout;
                                        if (onTimeout.isNotNull(_onTimeout))
                                            _onTimeout();
                                        completer->complete();
                                    });
    return completer->future;
}

ref<Future<std::nullptr_t>> Future<void>::than(Function<void()> fn)
{
    ref<Future<void>> self = self();
    this->_callbackHandler->post([this, self, fn]
                                 {
                                     if (this->_completed == false)
                                         this->_callbackList->emplace_back(fn);
                                     else
                                         fn();
                                 });
    return self;
}

ref<Future<void>> Future<void>::timeout(Duration duration, option<Fn<void()>> onTimeout)
{
    ref<Future<void>> self = self();
    ref<Completer<void>> completer = Object::create<Completer<void>>(this->_callbackHandler);
    Future<void>::delay(this->_callbackHandler, duration)
        ->than([completer, onTimeout]
               {
                   if (completer->_isCompleted == false)
                   {
                       lateref<Fn<void()>> _onTimeout;
                       if (onTimeout.isNotNull(_onTimeout))
                           _onTimeout();
                       completer->completeSync();
                   }
               });
    this->than([completer]
               {
                   if (completer->_isCompleted == false)
                       completer->completeSync();
               });
    return completer->future;
}

/**
 * @brief AsyncSnapshot implement
 * 
 */

const List<AsyncSnapshot<>::ConnectionState::Value>
    AsyncSnapshot<>::ConnectionState::values = {
        AsyncSnapshot<>::ConnectionState::none,
        AsyncSnapshot<>::ConnectionState::active,
        AsyncSnapshot<>::ConnectionState::done,
};

ref<String> AsyncSnapshot<>::ConnectionState::toString(AsyncSnapshot<>::ConnectionState::Value value)
{
    switch (value)
    {
    case AsyncSnapshot<>::ConnectionState::none:
        return "AsyncSnapshot<>::ConnectionState::none";
    case AsyncSnapshot<>::ConnectionState::active:
        return "AsyncSnapshot<>::ConnectionState::active";
    case AsyncSnapshot<>::ConnectionState::done:
        return "AsyncSnapshot<>::ConnectionState::done";
    default:
        assert(false && "The enum doesn't exists. ");
        return "The enum doesn't exists";
    }
}