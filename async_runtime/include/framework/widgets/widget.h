#pragma once

#include "key.h"
#include "../elements/stateless_element.h"
#include "../elements/stateful_element.h"

class Widget : public virtual Object
{
    Widget() = delete;

public:
    Widget(Object::Ref<Key> key = nullptr);
    virtual Object::Ref<Key> getKey();
    virtual bool equal(Object::Ref<Widget> other)
    {
        if (other == nullptr)
            return false;
        return this->runtimeType() == other->runtimeType() && this->getKey() == other->getKey();
    }

    friend StatefulElement;
    friend StatelessElement;
    friend RootElement;

protected:
    virtual Object::Ref<Element> createElement() = 0;
    Object::Ref<Key> _key;
};

class LeafWidget : public virtual Widget
{
public:
    LeafWidget(Object::Ref<Key> key = nullptr);
    virtual Object::Ref<Element> createElement() override;
};

class StatelessWidget : public Widget
{
public:
    StatelessWidget(Object::Ref<Key> key = nullptr);
    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) = 0;

protected:
    virtual Object::Ref<Element> createElement() override;
};

class StatefulWidget : public Widget
{
public:
    StatefulWidget(Object::Ref<Key> key = nullptr);
    virtual Object::Ref<State<StatefulWidget>> createState() = 0;

protected:
    virtual Object::Ref<Element> createElement() override;
};

template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type *>
class State : public virtual Object
{
public:
    State(Object::Ref<BuildContext> &context_, Object::Ref<T> &widget_)
        : context(context_), widget(widget_), mounted(false) {}

    // @mustCallSuper
    virtual void initState() {}
    // @mustCallSuper
    virtual void didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget) {}
    // @mustCallSuper
    virtual void didDependenceChanged() {}
    // @mustCallSuper
    virtual void dispose() {}

    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) = 0;

    friend StatefulElement;

protected:
    Object::WeakRef<BuildContext> context;
    Object::WeakRef<T> widget;
    bool mounted;
};

inline Object::Ref<Key> Widget::getKey()
{
    return this->_key;
}