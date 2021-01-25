#pragma once

#include "widget.h"
#include "../basic/state.h"
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
    State() : mounted(false) {}

    // @mustCallSuper
    virtual void initState() {}
    // @mustCallSuper
    virtual void didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget) {}
    // @mustCallSuper
    virtual void didDependenceChanged() {}
    // @mustCallSuper
    virtual void dispose() {}
    // @immutable
    void setState(Function<void()> fn)
    {
        fn();
        this->getElement()->build();
    }

    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) = 0;

protected:
    virtual Object::Ref<BuildContext> getContext() const
    {
        return this->getElement();
    }

    virtual bool getMounted() const
    {
        return this->mounted;
    }

    Object::WeakRef<StatefulElement> _element; //

private:
    bool mounted;

    Object::Ref<StatefulElement> getElement() const
    {
        assert(this->mounted && "User can't access context before initState. You can try [didDependenceChanged] method to access context before first build. ");
        return this->_element.lock();
    }
};

template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type *>
class State : public State<StatefulWidget>
{
public:
protected:
    virtual Object::Ref<const T> getWidget()
    {
        Object::Ref<StatefulElement> element = this->_element.lock();
        assert(element);
        return element->_statefulWidget->cast<T>();
    }
};