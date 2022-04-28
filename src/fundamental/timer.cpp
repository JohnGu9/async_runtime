#include "async_runtime/fundamental/timer.h"

#include <algorithm>
#include <ctime>
extern "C"
{
#include <uv.h>
}

static void timer_cb(uv_timer_t *handle)
{
    const std::function<void()> *fn = reinterpret_cast<std::function<void()> *>(handle->data);
    (*fn)();
}

class Timer::_Timer : public Timer
{
public:
    uv_timer_t _handle;

    _Timer(ref<EventLoop> loop, Function<void(ref<Timer>)> fn) : Timer(loop, fn)
    {
        uv_timer_init(reinterpret_cast<uv_loop_t *>(loop->nativeHandle()), &_handle);
        _handle.data = nullptr;
    }
    void cancel() override;

    class _Delay;
    class _Periodic;
};

void Timer::_Timer::cancel()
{
    if (_handle.data != nullptr)
    {
        delete reinterpret_cast<std::function<void()> *>(_handle.data);
        _handle.data = nullptr;
    }
    uv_timer_stop(&_handle);
}

class Timer::_Timer::_Delay : public Timer::_Timer
{
public:
    Duration timeout;
    _Delay(ref<EventLoop> loop, Duration timeout, Function<void(ref<Timer>)> fn) : Timer::_Timer(loop, fn), timeout(timeout) {}
    void start() override
    {
        cancel();
        auto self = self();
        _handle.data = new std::function<void()>([self]
                                                 {
        auto cache = self;
        cache->_fn(cache);
        cache->cancel(); });
        uv_timer_start(&_handle, timer_cb, timeout.toMilliseconds(), 0);
    }
};

class Timer::_Timer::_Periodic : public Timer::_Timer
{
public:
    Duration interval;
    _Periodic(ref<EventLoop> loop, Duration interval, Function<void(ref<Timer>)> fn) : Timer::_Timer(loop, fn), interval(interval) {}

    void start() override
    {
        cancel();
        auto self = self();
        _handle.data = new std::function<void()>([self]
                                                 { auto cache = self; cache->_fn(cache); });
        uv_timer_start(&_handle, timer_cb, interval.toMilliseconds(), interval.toMilliseconds());
    }
};

ref<Timer> Timer::delay(Duration duration, Function<void(ref<Timer>)> fn, option<EventLoopGetterMixin> getter)
{
    return Object::create<Timer::_Timer::_Delay>(EventLoopGetterMixin::ensureEventLoop(getter), duration, fn);
}

ref<Timer> Timer::periodic(Duration interval, Function<void(ref<Timer>)> fn, option<EventLoopGetterMixin> getter)
{
    return Object::create<Timer::_Timer::_Periodic>(EventLoopGetterMixin::ensureEventLoop(getter), interval, fn);
}
