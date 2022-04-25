#include <atomic>
#include <thread>
extern "C"
{
#include <uv.h>
}

#include "async_runtime/fundamental/event_loop.h"

static void callOnce(uv_async_t *handle)
{
    auto fn = reinterpret_cast<Function<void()> *>(handle->data);
    (*fn)();
    uv_unref(reinterpret_cast<uv_handle_t *>(handle));
    delete fn;
    delete handle;
}

class EventLoop::_EventLoop : public EventLoop
{
protected:
    uv_loop_t _loop;

public:
    _EventLoop() noexcept : EventLoop() { uv_loop_init(&_loop); }
    ~_EventLoop() noexcept { close(); }
    void close() override
    {
        if (alive())
            uv_loop_close(&_loop);
    }
    bool alive() const noexcept override { return uv_loop_alive(&_loop); }
    void *nativeHandle() noexcept override { return &_loop; }
    void callSoon(Function<void()> fn) override
    {
        uv_async_t *async = new uv_async_t;
        uv_async_init(&_loop, async, callOnce);
        async->data = new Function<void()>(fn);
        uv_async_send(async);
    }
};

class _ThreadEventLoop : public EventLoop::_EventLoop
{
    using super = EventLoop::_EventLoop;
    static void close_event_loop(uv_async_t *handler)
    {
        auto instance = reinterpret_cast<_ThreadEventLoop *>(handler->data);
        uv_unref(reinterpret_cast<uv_handle_t *>(handler));
        instance->super::close();
    }

public:
    uv_async_t _async;
    std::thread *_thread;

    _ThreadEventLoop() noexcept : super()
    {
        uv_async_init(&_loop, &_async, close_event_loop); // maintains a handler that stop loop from automatically close
        _async.data = this;
    }

    void close() override
    {
        if (alive())
        {
            uv_async_send(&_async); // use async handler close the event loop
            _thread->join();
            delete _thread;
            _thread = nullptr;
        }
    }
};

class EventLoop::Handle::_Handle : public EventLoop::Handle
{
public:
    uv_async_t _async;

    _Handle(ref<EventLoop> loop)
    {
        uv_async_init(reinterpret_cast<uv_loop_t *>(loop->nativeHandle()), &_async, callOnce);
    }

    void dispose() override
    {
        uv_unref(reinterpret_cast<uv_handle_t *>(&_async));
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
    lateref<EventLoopGetterMixin> notNull;
    if (getter.isNotNull(notNull))
    {
        return notNull->eventLoop();
    }
    return EventLoop::runningEventLoop.assertNotNull();
}

ref<EventLoop> EventLoop::ensure() noexcept
{
    lateref<EventLoop> loop;
    if (!runningEventLoop.isNotNull(loop) || !loop->alive())
    {
        runningEventLoop = loop = Object::create<_EventLoop>();
    }
    return loop;
}

static void execute_async_fn(uv_async_t *handler)
{
    Function<void()> *fn = reinterpret_cast<Function<void()> *>(handler->data);
    (*fn)();
    uv_unref(reinterpret_cast<uv_handle_t *>(handler));
}

int EventLoop::run(Function<void()> fn)
{
    lateref<EventLoop> loop;
    if (runningEventLoop.isNotNull(loop))
    {
        auto eventLoop = reinterpret_cast<uv_loop_t *>(loop->nativeHandle());

        uv_async_t async;
        uv_async_init(eventLoop, &async, execute_async_fn);
        async.data = &fn;
        uv_async_send(&async);
        isRunning = true;
        const auto code = uv_run(eventLoop, UV_RUN_DEFAULT);
        isRunning = false;
        return code;
    }
    throw std::logic_error("No event loop register on this thread. Please run [EventLoop::ensure] before call [EventLoop::run]");
}

ref<EventLoop> EventLoop::createEventLoopOnNewThread(Function<void()> fn)
{
    lateref<_ThreadEventLoop> loop;
    bool done = false;
    std::mutex mutex;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lock(mutex);
    std::thread *thread = new std::thread([fn, &loop, &done, &cv]
                                          {
        EventLoop::runningEventLoop = loop = Object::create<_ThreadEventLoop>(); 
        done = true;
        cv.notify_all();
        EventLoop::run(fn); });
    cv.wait(lock, [&done]
            { return done; });
    loop->_thread = thread;
    return loop;
}
