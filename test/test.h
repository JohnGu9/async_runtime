#pragma once

#include "async_runtime.h"

class MainActivity : public StatefulWidget
{
    virtual Object::Ref<State> createState() override;
};

class _MainActivityState : public State<MainActivity>
{
    using super = State<MainActivity>;

    void didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget) override
    {
        Logger::of(this->getContext())->writeLine("_MainActivityState::didWidgetUpdated");
        super::didWidgetUpdated(oldWidget);
    }

    void didDependenceChanged() override
    {
        Scheduler::Handler handler = Scheduler::of(this->getContext());
        Logger::of(this->getContext())->writeLine("_MainActivityState::didDependenceChanged");
        Logger::of(this->getContext())->writeLine("Current scheduler handler is " + handler->runtimeType());
        super::didDependenceChanged();
    }

    void dispose() override
    {
        Logger::of(this->getContext())->writeLine("_MainActivityState::didDependenceChanged");
        super::dispose();
    }

    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override
    {
        Logger::of(context)->writeLine("_MainActivityState::didDependenceChanged");
        return LeafWidget::factory();
    }
};

inline Object::Ref<StatefulWidgetState> MainActivity::createState()
{
    return Object::create<_MainActivityState>();
}
