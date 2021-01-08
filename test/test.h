#pragma once

#include <iostream>
#include "async_runtime.h"

struct MainActivity : public StatefulWidget
{
    virtual Object::Ref<StatefulWidgetState> createState() override;
};

struct _MainActivityState : public State<MainActivity>
{
    void initState() override;
    void dispose() override;

    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override;
};

inline Object::Ref<StatefulWidgetState> MainActivity::createState()
{
    return Object::create<_MainActivityState>();
}

void _MainActivityState::initState()
{
    StatefulWidgetState::initState();
    debug_print("_MainActivityState::initState");
}

void _MainActivityState::dispose()
{
    debug_print("_MainActivityState::dispose");
    StatefulWidgetState::dispose();
}

inline Object::Ref<Widget> _MainActivityState::build(Object::Ref<BuildContext> context)
{
    debug_print("_MainActivityState::build");
    return LeafWidget::factory();
}