#pragma once

#include "widget.h"
#include "state.h"
#include "../basic/function.h"
#include "../elements/stateful_element.h"

class StatefulWidget : public Widget
{
public:
    StatefulWidget(option<Key> key = nullptr) : Widget(key) {}
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
    lateref<StatefulElement> _element;
    lateref<BuildContext> _context;

#ifdef _WIN32
public:
#else
protected:
#endif
    // @immutable
    void setState(Function<void()> fn);
    const bool &mounted = _mounted;
    const ref<BuildContext> &context = _context;
};

template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type *>
class State : public State<StatefulWidget>
{
    using super = State<StatefulWidget>;
    lateref<T> _widget;
#ifdef _WIN32
public:
#else
protected:
#endif
    const ref<T> &widget = _widget;
    void initState() override
    {
        super::initState();
        _widget = _element->_statefulWidget->covariant<T>();
    }

    // @mustCallSuper
    virtual void didWidgetUpdated(ref<T> oldWIdget) {}

    void dispose() override
    {
        Object::detach(_widget);
        super::dispose();
    }

private:
    void didWidgetUpdated(ref<StatefulWidget> oldWidget) override
    {
        super::didWidgetUpdated(oldWidget);
        _widget = _element->_statefulWidget->covariant<T>();
        this->didWidgetUpdated(oldWidget->covariant<T>());
    }
};
