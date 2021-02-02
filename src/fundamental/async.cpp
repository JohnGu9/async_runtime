#include "async_runtime/fundamental/async.h"

////////////////////////////
//
// ThreadPool implement
//
////////////////////////////

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
void ThreadPool::dispose(bool join)
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

    if (join)
    {
        for (std::thread &worker : workers)
            worker.join();
    }
}

void ThreadPool::detach()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop)
            return;
        stop = true;
    }
    condition.notify_all();

    for (std::thread &worker : workers)
        worker.detach();
}

////////////////////////////
//
// AutoReleaseThreadPool implement
//
////////////////////////////

Object::Ref<ThreadPool> AutoReleaseThreadPool::shared()
{
    static Object::Ref<ThreadPool> singleton = Object::create<AutoReleaseThreadPool>(1);
    return singleton;
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
        AsyncSnapshot<>::ConnectionState::waiting};

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
    case AsyncSnapshot<>::ConnectionState::waiting:
        return "AsyncSnapshot<>::ConnectionState::waiting";
    default:
        assert(false && "The enum doesn't exists. ");
        break;
    }
}