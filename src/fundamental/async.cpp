#include "async_runtime/fundamental/async.h"

////////////////////////////
//
// ThreadPool implement
//
////////////////////////////

Set<String> ThreadPool::_namePool = Set<String>::empty();

thread_local String ThreadPool::thisThreadName = String();

ThreadPool::ThreadPool(size_t threads, String name) : _stop(false), _name(name)
{
    if (this->_name == nullptr)
    {
        static const String prefix =
#ifdef DEBUG
            "ThreadPool#";
#else
            "";
#endif
        this->_name = prefix + size_t(this);
    }
    {
        Object::Ref<Lock::UniqueLock> lock = ThreadPool::_namePool.lock->uniqueLock();
        assert(ThreadPool::_namePool->find(this->_name) == ThreadPool::_namePool->end() && "ThreadPool name can't repeat");
        ThreadPool::_namePool->insert(this->_name);
    }
    this->onConstruction(threads);
}

void ThreadPool::onConstruction(size_t threads)
{
    assert(threads > 0);
    _workers.reserve(threads);
    for (size_t i = 0; i < threads; ++i)
        _workers.emplace_back(this->workerBuilder(i));
}

std::function<void()> ThreadPool::workerBuilder(size_t threadId)
{
    return [this, threadId] {
        ThreadPool::thisThreadName = this->childrenThreadName(threadId);
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        // Windows not support thread name yet for now
        // yeah, I'm lazy
#elif __APPLE__
        pthread_setname_np(ThreadPool::thisThreadName.c_str());
#elif __linux__
        pthread_setname_np(pthread_self(), ThreadPool::threadName.c_str());
#endif
        for (;;)
        {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(this->_queueMutex);
                this->_condition.wait(lock, [this] { return this->_stop || !this->_microTasks.empty() || !this->_tasks.empty(); });

                if ( // this->stop &&
                    this->_microTasks.empty() &&
                    this->_tasks.empty()) // always finish all task
                    return;

                if (!this->_microTasks.empty())
                {
                    task = std::move(this->_microTasks.front());
                    this->_microTasks.pop();
                }
                else
                {
                    task = std::move(this->_tasks.front());
                    this->_tasks.pop();
                }
            }
            task();
        }
    };
}

String ThreadPool::childrenThreadName(size_t id) { return this->name + "#" + id; }

size_t ThreadPool::threads() const { return this->_workers.size(); }

ThreadPool::~ThreadPool()
{
    {
        Object::Ref<Lock::UniqueLock> lock = ThreadPool::_namePool.lock->uniqueLock();
        ThreadPool::_namePool->erase(ThreadPool::_namePool->find(this->_name));
    }
    std::unique_lock<std::mutex> lock(_queueMutex);
    assert(this->_stop && "ThreadPool memory leak. ThreadPool release without call [dispose]");
}

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
}

void ThreadPool::detach()
{
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        if (_stop)
            return;
        _stop = true;
    }
    _condition.notify_all();

    for (std::thread &worker : _workers)
        worker.detach();
}

////////////////////////////
//
// AutoReleaseThreadPool implement
//
////////////////////////////

Object::Ref<AutoReleaseThreadPool> AutoReleaseThreadPool::factory(size_t threads)
{
    assert(threads > 0);
    Object::Ref<AutoReleaseThreadPool> instance = Object::create<AutoReleaseThreadPool>(MakeSharedOnly(0), threads);
    instance->_workers.reserve(threads);
    for (size_t i = 0; i < threads; ++i)
        instance->_workers.emplace_back(instance->workerBuilder(i));
    return instance;
}

AutoReleaseThreadPool::~AutoReleaseThreadPool()
{
    _stop = true;
    _condition.notify_all();
    if (not _join)
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
            assert(false && "ThreadPool call [dispose] that already is disposed. ");
            return;
        }
        _stop = true;
        _join = true;
    }
    _condition.notify_all();
    for (std::thread &worker : _workers)
        worker.join();
}

void AutoReleaseThreadPool::detach()
{
    {
        std::unique_lock<std::mutex> lock(_queueMutex);
        if (_stop)
            return;
        _stop = true;
    }
    _condition.notify_all();
}

void AutoReleaseThreadPool::onConstruction(size_t threads) {}

std::function<void()> AutoReleaseThreadPool::workerBuilder(size_t threadId)
{
    Object::Ref<AutoReleaseThreadPool> self = Object::cast<>(this);
    return [self, threadId] { self->ThreadPool::workerBuilder(threadId); };
}

////////////////////////////
//
// Future<std::nullptr_t> implement
//
////////////////////////////

bool Future<std::nullptr_t>::every(const Set<Object::Ref<Future<>>> &set, Function<bool(Object::Ref<Future<>>)> fn)
{
    for (auto &future : set)
    {
        if (!fn(future))
            return false;
    }
    return true;
}

bool Future<std::nullptr_t>::any(const Set<Object::Ref<Future<>>> &set, Function<bool(Object::Ref<Future<>>)> fn)
{
    for (auto &future : set)
    {
        if (fn(future))
            return true;
    }
    return false;
}

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

Object::Ref<Future<void>> Future<void>::race(State<StatefulWidget> *state, Set<Object::Ref<Future<>>> &&set)
{
    if (set->empty())
        return Future<void>::value(state);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(getHandlerfromState(state));
    for (auto &future : set)
        future->than([completer] { completer->complete(); });
    return completer->future;
}

Object::Ref<Future<void>> Future<void>::wait(State<StatefulWidget> *state, Set<Object::Ref<Future<>>> &&set)
{
    size_t size = set->size();
    if (size == 0)
        return Future<void>::value(state);
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(getHandlerfromState(state));
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

Object::Ref<Future<void>> Future<void>::value(Object::Ref<ThreadPool> callbackHandler)
{
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(callbackHandler);
    completer->complete();
    return completer->future;
}

Object::Ref<Future<void>> Future<void>::value(State<StatefulWidget> *state)
{
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(state);
    completer->complete();
    return completer->future;
}

#include "async_runtime/fundamental/timer.h"
Object::Ref<Future<void>> Future<void>::delay(Object::Ref<ThreadPool> callbackHandler, Duration duration, Function<void()> onTimeout)
{
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(callbackHandler);
    Object::Ref<Timer> timer = Timer::delay(callbackHandler, duration, [completer, onTimeout] {
        if (onTimeout != nullptr)
            onTimeout();
        completer->complete();
    });
    return completer->future;
}

Object::Ref<Future<void>> Future<void>::delay(State<StatefulWidget> *state, Duration duration, Function<void()> onTimeout)
{
    Object::Ref<Completer<void>> completer = Object::create<Completer<void>>(state);
    Object::Ref<Timer> timer = Timer::delay(state, duration, [completer, onTimeout] {
        if (onTimeout != nullptr)
            onTimeout();
        completer->complete();
    });
    return completer->future;
}

Object::Ref<Future<std::nullptr_t>> Future<void>::than(Function<void()> fn)
{
    Object::Ref<Future<void>> self = Object::cast<>(this);
    this->_callbackHandler->post([self, fn] {
        if (self->_completed == false)
            self->_callbackList->push_back(fn);
        else
            fn();
    });
    return self;
}

Object::Ref<Future<void>> Future<void>::timeout(Duration, Function<void()> onTimeout)
{
    //TODO: implement function
    return Object::cast<>(this);
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

String AsyncSnapshot<>::ConnectionState::toString(AsyncSnapshot<>::ConnectionState::Value value)
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
        break;
    }
}