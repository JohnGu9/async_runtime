#pragma once

#include "../widgets/state.h"
#include "single_child_element.h"

class StatefulElement : public SingleChildElement
{
    template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type *>
    friend class State;

    class InvalidWidget;

public:
    StatefulElement(ref<StatefulWidget> widget);

    ref<StatefulWidget> _statefulWidget;
    ref<State<StatefulWidget>> _state;

protected:
    class LifeCycle
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

    LifeCycle::Value _lifeCycle;

    void attach() override;
    void detach() override;
    void build() override;
    void notify(ref<Widget> newWidget) override;
    void update(ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(ref<Element>)>) override;
};
