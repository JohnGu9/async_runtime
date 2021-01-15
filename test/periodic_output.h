#pragma once

#include "async_runtime.h"

class PeriodicOutput : public StatefulWidget
{
public:
    PeriodicOutput(Object::Ref<Widget> child_, Logger::Handler handler_, Object::Ref<Key> key = nullptr)
        : child(child_), handler(handler_), StatefulWidget(key) {}

    Object::Ref<Widget> child;
    Logger::Handler handler;
    Object::Ref<State> createState() override;
};

class _PeriodicOutputState : public State<PeriodicOutput>
{
    using super = State<PeriodicOutput>;
    Object::Ref<Timer> _timer;

    void initState() override
    {
        super::initState();
        debug_print("initState");
        _timer = Timer::periodic(
            this, [this] {
                Logger::of(this->getContext())->writeLine("Timer callback");
            },
            Duration(5000));
    }

    void dispose() override
    {
        debug_print("dispose");
        _timer->dispose();
        super::dispose();
    }

    Object::Ref<Widget> build(Object::Ref<BuildContext>) override
    {
        return this->getWidget()->child;
    }
};

inline Object::Ref<StatefulWidgetState> PeriodicOutput::createState()
{
    return Object::create<_PeriodicOutputState>();
}