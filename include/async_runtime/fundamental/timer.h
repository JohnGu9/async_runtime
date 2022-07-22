#pragma once

#include "../utilities/duration.h"
#include "event_loop.h"

class Timer : public virtual Object, public EventLoopGetterMixin
{
public:
    static ref<Timer> delay(Duration duration, Function<void(ref<Timer>)> fn, option<EventLoopGetterMixin> getter = nullptr);
    static ref<Timer> delay(Duration duration, Function<void()> fn, option<EventLoopGetterMixin> getter = nullptr);
    static ref<Timer> periodic(Duration interval, Function<void(ref<Timer>)> fn, option<EventLoopGetterMixin> getter = nullptr);
    static ref<Timer> periodic(Duration interval, Function<void()> fn, option<EventLoopGetterMixin> getter = nullptr);
    virtual void start() = 0;
    virtual void cancel() noexcept = 0;

    ref<EventLoop> eventLoop() override;

protected:
    Timer(ref<EventLoop> loop, Function<void(ref<Timer>)> fn) : _loop(loop), _fn(fn) {}

    ref<EventLoop> _loop;
    Function<void(ref<Timer>)> _fn;

    class _Timer;
};
