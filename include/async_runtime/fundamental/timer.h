#pragma once

#include <atomic>
#include "async.h"
#include "dispatcher.h"
#include "../basic/function.h"
#include "../basic/duration.h"

class Timer : public Dispatcher
{
    struct _CreateOnly
    {
        constexpr _CreateOnly() {}
    };

public:
    Timer(State<StatefulWidget> *state, const _CreateOnly &);
    Timer(Object::Ref<ThreadPool> callbackHandler, const _CreateOnly &);
    static Object::Ref<Timer> delay(State<StatefulWidget> *state, Duration duration, Function<void()> fn);
    static Object::Ref<Timer> periodic(State<StatefulWidget> *state, Duration interval, Function<void()> fn);
    static Object::Ref<Timer> delay(Object::Ref<ThreadPool> callbackHandler, Duration duration, Function<void()> fn);
    static Object::Ref<Timer> periodic(Object::Ref<ThreadPool> callbackHandler, Duration interval, Function<void()> fn);

    virtual ~Timer();

    virtual void cancel();
    void dispose() override;

protected:
    std::shared_ptr<std::atomic_bool> _clear;

    // now Timer can't reuse
    // Please create new Timer for new task
    virtual void _setTimeout(Duration delay, Function<void()> function);
    virtual void _setInterval(Duration interval, Function<void()> function);
};
