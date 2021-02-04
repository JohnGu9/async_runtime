#pragma once

#include "widget.h"
#include "state.h"
#include "../basic/function.h"

class StatefulWidget : public Widget
{
public:
    StatefulWidget(Object::Ref<Key> key = nullptr) : Widget(key) {}
    virtual Object::Ref<State<StatefulWidget>> createState() = 0;

protected:
    Object::Ref<Element> createElement() override;
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
    virtual void didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget) {}
    // @mustCallSuper
    virtual void didDependenceChanged() {}
    // @mustCallSuper
    virtual void dispose() {}


    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) = 0;

private:
    bool _mounted;
    Object::Ref<StatefulElement> _element;
    Object::Ref<BuildContext> _context;

protected:
    // @immutable
    void setState(Function<void()> fn)
    {
        fn();
        this->_element->build();
    }

    const bool &mounted = _mounted;
    const Object::Ref<BuildContext> &context = _context;
};

template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type *>
class State : public State<StatefulWidget>
{
public:
protected:
    virtual Object::Ref<const T> getWidget()
    {
        return _element->_statefulWidget->cast<T>();
    }
};