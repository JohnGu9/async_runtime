#pragma once

#include <iostream>
#include "async_runtime.h"

struct MainActivity : public StatefulWidget
{
    virtual Object::Ref<StatefulWidgetState> createState() override;
};

struct _MainActivityState : public State<MainActivity>
{
    using super = StatefulWidgetState;

    void initState() override;
    void dispose() override;
    void didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget) override;
    void didDependenceChanged() override;

    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override;
};

inline Object::Ref<StatefulWidgetState> MainActivity::createState()
{
    return Object::create<_MainActivityState>();
}

void _MainActivityState::initState()
{
    super::initState();
    debug_print("_MainActivityState::initState");
}

void _MainActivityState::dispose()
{
    debug_print("_MainActivityState::dispose");
    super::dispose();
}

inline void _MainActivityState::didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget)
{
    debug_print("_MainActivityState::didWidgetUpdated");
    super::didWidgetUpdated(oldWidget);
}

inline void _MainActivityState::didDependenceChanged()
{
    debug_print("_MainActivityState::didDependenceChanged");
    super::didDependenceChanged();
}

inline Object::Ref<Widget> _MainActivityState::build(Object::Ref<BuildContext> context)
{
    debug_print("_MainActivityState::build");
    return LeafWidget::factory();
}
