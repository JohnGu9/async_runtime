#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <vector>

#include "async.h"
#include "event_loop.h"
#include "thread.h"

class AsyncThreadPool : public virtual Object
{
public:
    class Single;

    static ref<AsyncThreadPool::Single> single(bool autoDispose = false);
    static ref<AsyncThreadPool> fromThreadCount(size_t n, bool autoDispose = false);

    template <typename T>
    ref<Future<T>> withContext(Function<T()> fn, option<EventLoopGetterMixin> loop = nullptr);

    virtual void launch(Function<void()> fn);
    virtual bool isActive();
    virtual void dispose(bool dropRemainTasks = false) = 0;

    AsyncThreadPool(bool autoDispose);
    virtual ~AsyncThreadPool();

protected:
    // the task queue
    std::queue<Function<void()>> _tasks;

    // synchronization
    std::mutex _mutex;
    std::condition_variable _condition;
    bool _stop = false;
    bool _autoDispose;

    class Multi;
};

class AsyncThreadPool::Single : public AsyncThreadPool
{
    using super = AsyncThreadPool;
    bool _dropRemainTasks = false;

public:
    Thread _worker;

    Single(bool autoDispose);
    ~Single();
    void dispose(bool dropRemainTasks = false) override;
};

template <typename T>
ref<Future<T>> AsyncThreadPool::withContext(Function<T()> fn, option<EventLoopGetterMixin> getter)
{
    auto loop = EventLoop::ensureEventLoop(getter);
    finalref<Completer<T>> completer = Object::create<Completer<T>>(loop);
    this->launch([completer, fn, loop] { //
        completer->setResult(fn());
        loop->callSoonThreadSafe([completer] { //
            completer->markAsCompleted();
        });
    });
    return completer;
}
