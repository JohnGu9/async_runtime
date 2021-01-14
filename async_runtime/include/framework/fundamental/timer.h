#pragma once

#include <atomic>
#include "dispatcher.h"
#include "../basic/function.h"
#include "../basic/duration.h"

class Timer : public Dispatcher
{
public:
    Timer(StatefulWidget::State *state);
    virtual ~Timer();

    virtual void setTimeout(Fn<void()> function, Duration delay);
    virtual void setInterval(Fn<void()> function, Duration interval);
    virtual void cancel();

    void dispose() override;

protected:
    Object::Ref<std::atomic_bool> _clear;
};