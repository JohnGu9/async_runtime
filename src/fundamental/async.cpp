#include "async_runtime/fundamental/async.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#include <processthreadsapi.h>
#endif

////////////////////////////
//
// ThreadPool implement
//
////////////////////////////

static ref<String> emptyString = "";

Set<ref<String>> ThreadPool::_namePool = Set<ref<String>>::empty();

thread_local ref<String> ThreadPool::thisThreadName = "MainThread";

ThreadPool::ThreadPool(size_t threads, option<String> name) : _stop(false), _name(emptyString)
{
    lateref<String> n;
    if (name.isNotNull(n))
    {
        this->_name = n;
    }
    else
    {
        static finalref<String> prefix = "ThreadPool#";
        this->_name = prefix + size_t(this);
    }

    {
        assert(this->_name->isNotEmpty());
        option<Lock::UniqueLock> lock = ThreadPool::_namePool.lock->uniqueLock();
        assert(ThreadPool::_namePool->find(this->_name) == ThreadPool::_namePool->end() && "ThreadPool name can't repeat");
        ThreadPool::_namePool->insert(this->_name);
    }
    _workers.reserve(threads);
    for (size_t i = 0; i < threads; ++i)
        _workers.emplace_back(ThreadPool::workerBuilder(i));
}

ThreadPool::~ThreadPool(){
#ifdef DEBUG
    {option<Lock::UniqueLock> lock = ThreadPool::_namePool.lock->uniqueLock();
assert(ThreadPool::_namePool->find(this->_name) == ThreadPool::_namePool->end());
}
{
    std::unique_lock<std::mutex> lock(_queueMutex);
    assert(this->_stop && "ThreadPool memory leak. ThreadPool release without call [dispose]");
}
#endif
}

std::function<void()> ThreadPool::workerBuilder(size_t threadId)
{
    return [this, threadId] {
        ThreadPool::thisThreadName = this->childrenThreadName(threadId);
        assert(ThreadPool::thisThreadName->isNotEmpty());

#ifdef DEBUG
        const std::string debugThreadName = ThreadPool::thisThreadName->toStdString();
        const std::string debugThreadPoolRuntimeType = this->runtimeType()->toStdString();
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        SetThreadDescription(GetCurrentThread(), ThreadPool::thisThreadName->c_str());
#elif __APPLE__
        pthread_setname_np(ThreadPool::thisThreadName->c_str());
#elif __linux__
        pthread_setname_np(pthread_self(), ThreadPool::thisThreadName->c_str());
#endif
#endif

        while (true)
        {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(this->_queueMutex);
                this->_condition.wait(lock, [this] {
                    return this->_stop || !this->_microTasks.empty() || !this->_tasks.empty();
                });

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

ref<String> ThreadPool::childrenThreadName(size_t id) { return this->name + "#" + id; }

size_t ThreadPool::threads() const { return this->_workers.size(); }

bool ThreadPool::isActive()
{
    std::unique_lock<std::mutex> lock(_queueMutex);
    return !this->_stop;
}

// the destructor joins all threads
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
    option<Lock::UniqueLock> lock = ThreadPool::_namePool.lock->uniqueLock();
    ThreadPool::_namePool->erase(ThreadPool::_namePool->find(this->_name));
}

////////////////////////////
//
// AutoReleaseThreadPool implement
//
////////////////////////////

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
    unregisterName();
}

void AutoReleaseThreadPool::close()
{
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        _stop = true;
    }
    _condition.notify_all();
    unregisterName();
}

std::function<void()> AutoReleaseThreadPool::workerBuilder(size_t threadId)
{
    ref<AutoReleaseThreadPool> self = self();
    return [self, threadId] { self->ThreadPool::workerBuilder(threadId)(); };
}

////////////////////////////
//
// Future<std::nullptr_t> implement
//
////////////////////////////

void Future<std::nullptr_t>::sync()
{
    std::mutex mutex;
    std::condition_variable condition;
    std::unique_lock<std::mutex> lock(mutex);
    this->than([&] { condition.notify_all(); });
    condition.wait(lock);
}

void Future<std::nullptr_t>::sync(Duration timeout)
{
    std::mutex mutex;
    std::condition_variable condition;
    std::unique_lock<std::mutex> lock(mutex);
    this->than([&] { condition.notify_all(); });
    condition.wait(lock);
}

////////////////////////////
//
// Future<void> implement
//
////////////////////////////

ref<Future<void>> Future<void>::race(State<StatefulWidget> *state, Set<ref<Future<>>> set)
{
    if (set->empty())
        return Future<void>::value(state);
    ref<Completer<void>> completer = Object::create<Completer<void>>(getHandlerfromState(state));
    for (auto &future : set)
        future->than([completer] { completer->complete(); });
    return completer->future;
}

ref<Future<void>> Future<void>::wait(State<StatefulWidget> *state, Set<ref<Future<>>> set)
{
    size_t size = set->size();
    if (size == 0)
        return Future<void>::value(state);
    ref<Completer<void>> completer = Object::create<Completer<void>>(getHandlerfromState(state));
    size_t *count = new size_t(0);
    for (auto &future : set)
        future->than([completer, count, size] {
            completer->_callbackHandler->post([completer, count, size] {
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

ref<Future<void>> Future<void>::value(State<StatefulWidget> *state)
{
    ref<Completer<void>> completer = Object::create<Completer<void>>(state);
    completer->complete();
    return completer->future;
}

#include "async_runtime/fundamental/timer.h"
ref<Future<void>> Future<void>::delay(ref<ThreadPool> callbackHandler, Duration duration, Function<void()> onTimeout)
{
    ref<Completer<void>> completer = Object::create<Completer<void>>(callbackHandler);
    ref<Timer> timer = Timer::delay(callbackHandler, duration, [completer, onTimeout] {
        if (onTimeout != nullptr)
            onTimeout();
        completer->complete();
    });
    return completer->future;
}

ref<Future<void>> Future<void>::delay(State<StatefulWidget> *state, Duration duration, Function<void()> onTimeout)
{
    ref<Completer<void>> completer = Object::create<Completer<void>>(state);
    ref<Timer> timer = Timer::delay(state, duration, [completer, onTimeout] {
        if (onTimeout != nullptr)
            onTimeout();
        completer->complete();
    });
    return completer->future;
}

ref<Future<std::nullptr_t>> Future<void>::than(Function<void()> fn)
{
    ref<Future<void>> self = self();
    this->_callbackHandler->post([self, fn] {
        if (self->_completed == false)
            self->_callbackList->emplace_back(fn);
        else
            fn();
    });
    return self;
}

ref<Future<void>> Future<void>::timeout(Duration duration, Function<void()> onTimeout)
{
    ref<Future<void>> self = self();
    ref<Completer<void>> completer = Object::create<Completer<void>>(this->_callbackHandler);
    Future<void>::delay(this->_callbackHandler, duration)
        ->than([=] {
            if (completer->_isCompleted == false)
            {
                if (onTimeout != nullptr)
                    onTimeout();
                completer->completeSync();
            }
        });
    this->than([=] {
        if (completer->_isCompleted == false)
            completer->completeSync();
    });
    return completer->future;
}

////////////////////////////
//
// AsyncSnapshot implement
//
////////////////////////////

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