#pragma once

#include <atomic>
#include "dispatcher.h"
#include "../basic/function.h"
#include "../basic/duration.h"

class Timer : public Dispatcher
{
public:
    Timer(State<StatefulWidget> *state);
    static Object::Ref<Timer> delay(State<StatefulWidget> *state, Fn<void()> fn, Duration duration);
    static Object::Ref<Timer> periodic(State<StatefulWidget> *state, Fn<void()> fn, Duration interval);

    virtual ~Timer();

    virtual void setTimeout(Fn<void()> function, Duration delay);
    virtual void setInterval(Fn<void()> function, Duration interval);
    virtual void cancel();

    void dispose() override;

protected:
    Object::Ref<std::atomic_bool> _clear;
};