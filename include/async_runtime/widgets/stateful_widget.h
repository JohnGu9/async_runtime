#pragma once

#include "widget.h"
#include "state.h"
#include "../basic/function.h"
#include "../elements/stateful_element.h"

class StatefulWidget : public Widget
{
public:
    StatefulWidget(ref<Key> key = nullptr) : Widget(key) {}
    virtual ref<State<StatefulWidget>> createState() = 0;

protected:
    ref<Element> createElement() override;
};

template <>
class State<StatefulWidget> : public Object
{
    friend class StatefulElement;
    friend class StateHelper;

    template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type *>
    friend class State;

public:
    State() : _mounted(false) {}

    // @mustCallSuper
    virtual void initState() {}
    // @mustCallSuper
    virtual void didWidgetUpdated(ref<StatefulWidget> oldWidget) {}
    // @mustCallSuper
    virtual void didDependenceChanged() {}
    // @mustCallSuper
    virtual void dispose() {}

    virtual ref<Widget> build(ref<BuildContext> context) = 0;

private:
    bool _mounted;
    ref<StatefulElement> _element;
    ref<BuildContext> _context;

protected:
    // @immutable
    void setState(Function<void()> fn);

    const bool &mounted = _mounted;
    const ref<BuildContext> &context = _context;
};

template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type *>
class State : public State<StatefulWidget>
{
public:
protected:
    virtual ref<const T> getWidget()
    {
        return _element->_statefulWidget->cast<T>();
    }
};
