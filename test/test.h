#pragma once

#include <iostream>
#include "async_runtime.h"

struct MainActivity : public StatefulWidget
{
    virtual Object::Ref<StatefulWidgetState> createState() override;
};

struct _MainActivityState : public State<MainActivity>
{
    Object::Ref<Widget> build(Object::Ref<BuildContext> context) override;
};

inline Object::Ref<StatefulWidgetState> MainActivity::createState()
{
    return Object::create<_MainActivityState>();
}

inline Object::Ref<Widget> _MainActivityState::build(Object::Ref<BuildContext> context)
{
    debug_print("_MainActivityState::build");
    return Object::create<LeafWidget>();
}