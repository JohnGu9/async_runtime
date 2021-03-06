#pragma once

#include <atomic>
#include "async.h"
#include "dispatcher.h"
#include "../basic/duration.h"

class Timer : public Dispatcher
{
    struct _FactoryOnly
    {
        constexpr _FactoryOnly() {}
    };

public:
    class TimerThread;

    static ref<Timer> delay(ref<State<StatefulWidget>> state, Duration duration, Function<void()> fn);
    static ref<Timer> periodic(ref<State<StatefulWidget>> state, Duration interval, Function<void()> fn);
    static ref<Timer> delay(ref<ThreadPool> callbackHandler, Duration duration, Function<void()> fn);
    static ref<Timer> periodic(ref<ThreadPool> callbackHandler, Duration interval, Function<void()> fn);

    Timer(ref<State<StatefulWidget>> state, const _FactoryOnly &);
    Timer(ref<ThreadPool> callbackHandler, const _FactoryOnly &);

    virtual ~Timer();
    virtual void cancel();

protected:
    void dispose() override;

    std::mutex _mutex;
    std::atomic_bool _clear;
    lateref<Fn<void()>> _onCancel;

    // now Timer can't reuse
    // Please create new Timer for new task
    virtual void _setTimeout(Duration delay, Function<void()> function);
    virtual void _setInterval(Duration interval, Function<void()> function);
};
