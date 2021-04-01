#pragma once

#include "single_child_element.h"

class StatefulWidget;
class StatefulElement : public SingleChildElement
{
    template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type *>
    friend class State;

public:
    StatefulElement(ref<StatefulWidget> widget);

    lateref<StatefulWidget> _statefulWidget;
    lateref<State<StatefulWidget> > _state;

protected:
    class _LifeCycle
    {
    public:
        enum Value
        {
            uninitialized,
            mounted,
            building,
            unmount,
        };
        static const List<Value> values;
        static ref<String> toString(Value value);
    };

    _LifeCycle::Value _lifeCycle;

    void attach() override;
    void detach() override;
    void build() override;
    void notify(ref<Widget> newWidget) override;
    void update(ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(ref<Element>)>) override;
};