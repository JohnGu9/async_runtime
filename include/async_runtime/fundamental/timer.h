#pragma once

#include <atomic>
#include "async.h"
#include "disposable.h"
#include "../basic/function.h"
#include "../basic/duration.h"

class Timer : public Object, StateHelper, public Disposable
{
public:
    Timer(State<StatefulWidget> *state);
    static Object::Ref<Timer> delay(State<StatefulWidget> *state, Duration duration, Function<void()> fn);
    static Object::Ref<Timer> periodic(State<StatefulWidget> *state, Duration interval, Function<void()> fn);

    virtual ~Timer();

    virtual void setTimeout(Duration delay, Function<void()> function);
    virtual void setInterval(Duration interval, Function<void()> function);
    virtual void cancel();

    void dispose();

protected:
    Object::Ref<AutoReleaseThreadPool> _autoReleaseThreadPool;
    Object::Ref<ThreadPool> _callbackHandler;
    std::shared_ptr<std::atomic_bool> _clear;
};