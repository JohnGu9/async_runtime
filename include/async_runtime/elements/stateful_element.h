#pragma once

#include "../fundamental/async.h"
#include "../widgets/state.h"
#include "single_child_element.h"

class StatefulElement : public SingleChildElement
{
    template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type *>
    friend class State;

    class InvalidWidget;

public:
    class LifeCycle
    {
    public:
        enum Value : int
        {
            uninitialized,
            mounted,
            building,
            unmount,
        };
        static const ref<List<Value>> values;
        static ref<String> toString(Value value);
    };

    StatefulElement(ref<StatefulWidget> widget);

    ref<StatefulWidget> _statefulWidget;
    ref<State<StatefulWidget>> _state;
    ref<List<Function<void()>>> _setStateCallbacks;
    LifeCycle::Value _lifeCycle;

    void attach() override;
    void detach() override;
    void build() override;
    void notify(ref<Widget> newWidget) override;
    void update(ref<Widget> newWidget) override;
    void visitDescendant(Function<bool(ref<Element>)>) override;

    void scheduleRebuild(Function<void()> fn, ref<EventLoop> loop);
};

namespace std
{
    /**
     * @brief Before C++14, there is no standard for enum convert to hash.
     * In Windows and Linux, system provide the way.
     * But Macos doesn't.
     *
     */
    template <>
    struct hash<::StatefulElement::LifeCycle::Value>
    {
        std::size_t operator()(const ::StatefulElement::LifeCycle::Value &other) const
        {
            return ::std::hash<int>()(static_cast<int>(other));
        }
    };
};
