#pragma once

#include <atomic>
#include "dispatcher.h"
#include "../basic/function.h"
#include "../basic/duration.h"

class Timer : public Dispatcher
{
public:
    Timer(State<StatefulWidget> *state);
    static Object::Ref<Timer> delay(State<StatefulWidget> *state, Function<void()> fn, Duration duration);
    static Object::Ref<Timer> periodic(State<StatefulWidget> *state, Function<void()> fn, Duration interval);

    virtual ~Timer();

    virtual void setTimeout(Function<void()> function, Duration delay);
    virtual void setInterval(Function<void()> function, Duration interval);
    virtual void cancel();

    void dispose() override;

protected:
    Object::Ref<std::atomic_bool> _clear;
};