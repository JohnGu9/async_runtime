#include <atomic>
#include <list>
extern "C"
{
#include <uv.h>
}

#include "async_runtime/fundamental/event_loop.h"
#include "async_runtime/fundamental/thread.h"
#include "async_runtime/utilities/lock.h"

class EventLoop::_EventLoop : public EventLoop
{
protected:
    static void clear_tasks_queue(uv_async_t *handle)
    {
        auto loop = reinterpret_cast<EventLoop::_EventLoop *>(handle->data);
        auto &tasks = loop->_tasks_queue;
        lateref<Fn<void()>> fn;
        while (!tasks.empty())
        {
            fn = tasks.front();
            tasks.pop_front();
            fn();
        }
    }

    static void clear_tasks_queue_thread_safe(uv_async_t *handle)
    {
        auto loop = reinterpret_cast<EventLoop::_EventLoop *>(handle->data);
        auto &tasks = loop->_tasks_queue_thread_safe;
        auto &mtx = loop->mtx;
        lateref<Fn<void()>> fn;
        while (true)
        {
            {
                std::unique_lock<std::mutex> lock(mtx);
                if (tasks.empty())
                    break;
                fn = tasks.front();
                tasks.pop_front();
            }
            fn();
        }
    }

    uv_loop_t _loop;
    uv_async_t _clear_tasks_queue;
    std::list<Function<void()>> _tasks_queue;

    std::mutex mtx;
    uv_async_t _clear_tasks_queue_thread_safe;
    std::list<Function<void()>> _tasks_queue_thread_safe;

public:
    _EventLoop() noexcept : EventLoop()
    {
        uv_loop_init(&_loop);

        uv_async_init(&_loop, &_clear_tasks_queue, clear_tasks_queue);
        _clear_tasks_queue.data = this;
        uv_unref(reinterpret_cast<uv_handle_t *>(&_clear_tasks_queue));

        uv_async_init(&_loop, &_clear_tasks_queue_thread_safe, clear_tasks_queue_thread_safe);
        _clear_tasks_queue_thread_safe.data = this;
        uv_unref(reinterpret_cast<uv_handle_t *>(&_clear_tasks_queue_thread_safe));
    }
    void *nativeHandle() noexcept override { return &_loop; }

    void close() override {}
    bool alive() noexcept override { return uv_loop_alive(&_loop); }

    void callSoon(Function<void()> fn) noexcept override
    {
        this->_tasks_queue.emplace_back(fn);
        uv_async_send(&_clear_tasks_queue);
    }
    void callSoonThreadSafe(Function<void()> fn) noexcept override
    {
        {
            std::unique_lock<std::mutex> lock(mtx);
            this->_tasks_queue_thread_safe.emplace_back(fn);
        }
        uv_async_send(&_clear_tasks_queue_thread_safe);
    }
};

class _ThreadEventLoop : public EventLoop::_EventLoop
{
    using super = EventLoop::_EventLoop;
    static void close_event_loop(uv_async_t *handle)
    {
        uv_unref(reinterpret_cast<uv_handle_t *>(handle));
    }

public:
    uv_async_t _close_signal;
    std::thread _thread;

    bool _alive = true;
    ref<Lock> _lock;

    _ThreadEventLoop() noexcept : super(), _lock(Object::create<Lock>())
    {
        uv_async_init(&_loop, &_close_signal, close_event_loop); // maintains a handler that stop loop from automatically close
        _close_signal.data = this;
    }

    bool alive() noexcept override
    {
        auto lk = _lock->sharedLock();
        return _alive;
    }

    void close() override
    {
        if (alive())
        {
            uv_async_send(&_close_signal); // use async handler close the event loop
            _thread.join();
            {
                auto lk = _lock->uniqueLock();
                _alive = false;
            }
        }
    }
};

class EventLoop::Handle::_Handle : public EventLoop::Handle
{
public:
    static void cleanup(uv_handle_t *handle)
    {
        delete reinterpret_cast<uv_async_t *>(handle);
    }

    static void cb(uv_async_t *handle)
    {
        uv_close(reinterpret_cast<uv_handle_t *>(handle), cleanup);
    }

    uv_async_t *_async;

    _Handle(ref<EventLoop> loop) : _async(new uv_async_t)
    {
        uv_async_init(reinterpret_cast<uv_loop_t *>(loop->nativeHandle()),
                      _async, cb);
    }

    virtual ~_Handle()
    {
        assert(_async == nullptr && "EventLoop::Handle::dispose should be called before ref dropped");
    }

    void dispose() override
    {
        uv_async_send(_async);
        _async = nullptr;
    }
};

ref<EventLoop::Handle> EventLoop::Handle::create(option<EventLoopGetterMixin> getter)
{
    return Object::create<EventLoop::Handle::_Handle>(EventLoopGetterMixin::ensureEventLoop(getter));
}

thread_local option<EventLoop> EventLoop::runningEventLoop;
thread_local bool EventLoop::isRunning = false;

ref<EventLoop> EventLoopGetterMixin::ensureEventLoop(option<EventLoopGetterMixin> getter)
{
    if (auto ptr = getter.get())
    {
        return ptr->eventLoop();
    }
    return EventLoop::runningEventLoop.assertNotNull();
}

ref<EventLoop> EventLoop::ensure() noexcept
{
    if_not_null(runningEventLoop)
    {
        if (runningEventLoop->alive())
            return runningEventLoop;
    }
    end_if();

    auto newLoop = Object::create<_EventLoop>();
    runningEventLoop = newLoop;
    return newLoop;
}

static void event_loop_main(uv_async_t *handler)
{
    Function<void()> *fn = reinterpret_cast<Function<void()> *>(handler->data);
    (*fn)();
    uv_close(reinterpret_cast<uv_handle_t *>(handler), 0);
}

void EventLoop::run(Function<void()> fn)
{
    ref<EventLoop> loop = ensure();
    if (isRunning)
    {
        loop->callSoon(fn);
    }
    else
    {
#ifndef NDEBUG
        {
            std::stringstream ss("");
            ss << "EventLoop[" << std::this_thread::get_id() << "]";
            ThreadUnit::setThreadName(ss.str());
        }
#endif
        auto eventLoop = reinterpret_cast<uv_loop_t *>(loop->nativeHandle());
        uv_async_t async;
        uv_async_init(eventLoop, &async, event_loop_main);
        async.data = &fn;
        uv_async_send(&async);
        isRunning = true;
        uv_run(eventLoop, UV_RUN_DEFAULT);
        uv_loop_close(eventLoop);
        isRunning = false;
    }
}

ref<EventLoop> EventLoop::createEventLoopOnNewThread(Function<void()> fn)
{
    bool done = false;
    lateref<_ThreadEventLoop> loop;
    std::mutex mutex;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(mutex);
    auto thread = std::thread(
        [fn, &done, &loop, &cv, &mutex] { //
            EventLoop::runningEventLoop = loop = Object::create<_ThreadEventLoop>();
            EventLoop::run([fn, &done, &cv, &mutex] { //
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    done = true;
                }
                cv.notify_all();
                fn();
            });
        });
    cv.wait(lock, [&done]
            { return done; });
    loop->_thread.swap(thread);
    return loop;
}
