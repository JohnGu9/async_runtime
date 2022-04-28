#pragma once

#include "../elements/stateful_element.h"
#include "../fundamental/async.h"
#include "state.h"
#include "widget.h"

class StatefulWidget : public Widget
{
public:
    StatefulWidget(option<Key> key = nullptr) : Widget(key) {}
    virtual ref<State<StatefulWidget>> createState() = 0;

protected:
    ref<Element> createElement() override;
};

template <>
class State<StatefulWidget> : public virtual Object, public EventLoopGetterMixin
{
    friend class StatefulElement;
    friend class StateHelper;

    template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type *>
    friend class State;

    ref<EventLoop> eventLoop() override { return EventLoopGetterMixin::ensureEventLoop(nullptr); }

    // @mustCallSuper
    virtual void initState() {}
    // @mustCallSuper
    virtual void didWidgetUpdated(ref<StatefulWidget> oldWidget) {}
    // @mustCallSuper
    virtual void didDependenceChanged() {}
    // @mustCallSuper
    virtual void dispose() {}

    virtual ref<Widget> build(ref<BuildContext> context) = 0;

    bool _mounted = false;
    lateref<StatefulElement> _element;
    lateref<BuildContext> _context;

protected:
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

protected:
    // @mustCallSuper
    void initState() override
    {
        super::initState();
        _widget = _element->_statefulWidget->covariant<T>();
    }

    // @mustCallSuper
    virtual void didWidgetUpdated(ref<T> oldWidget) {}

    // @mustCallSuper
    void didDependenceChanged() override { super::didDependenceChanged(); }

    // @mustCallSuper
    void dispose() override
    {
        Object::detach(_widget);
        super::dispose();
    }

    const ref<T> &widget = _widget;

private:
    void didWidgetUpdated(ref<StatefulWidget> oldWidget) final
    {
        super::didWidgetUpdated(oldWidget);
        _widget = _element->_statefulWidget->covariant<T>();
        this->didWidgetUpdated(oldWidget->covariant<T>());
    }
};
