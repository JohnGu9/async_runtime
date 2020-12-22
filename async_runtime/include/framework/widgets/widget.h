#pragma once

#include "key.h"
#include "../contexts/build_context.h"
#include "../elements/stateless_element.h"
#include "../elements/stateful_element.h"

class Widget : public virtual Object
{
    Widget() = delete;

public:
    Widget(Object::Ref<Key> key = nullptr);
    virtual Object::Ref<Key> getKey();
    virtual bool canUpdate(Object::Ref<Widget> other)
    {
        if (other == nullptr)
            return false;
        return this->runtimeType() == other->runtimeType() && this->getKey() == other->getKey();
    }

    virtual Object::Ref<Element> createElement() = 0;

protected:
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
    State() : mounted(false) {}

    // @mustCallSuper
    virtual void initState() {}
    // @mustCallSuper
    virtual void didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget) {}
    // @mustCallSuper
    virtual void didDependenceChanged() {}
    // @mustCallSuper
    virtual void dispose() {}

    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) = 0;

    void setState() {}

    friend StatefulElement;

protected:
    virtual Object::Ref<BuildContext> getContext()
    {
        assert(this->mounted); // can't access context before initState
        Object::Ref<StatefulElement> element = this->element.lock();
        return element;
    }

    virtual Object::Ref<T> getWidget()
    {
        Object::Ref<StatefulElement> element = this->element.lock();
        return element->_statefulWidget->cast<T>();
    }

    bool mounted;

private:
    Object::WeakRef<StatefulElement> element;
};

inline Object::Ref<Key> Widget::getKey()
{
    return this->_key;
}

class InheritedWidget : public virtual StatelessWidget, public virtual Inherit
{
public:
    InheritedWidget(Object::Ref<Widget> child) : _child(child) { assert(_child != nullptr); }
    virtual bool updateShouldNotify(Object::Ref<InheritedWidget> oldWidget) = 0;
    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) override;
    virtual Object::Ref<Element> createElement() override;

protected:
    Object::Ref<Widget> _child;
};

template <typename T, typename std::enable_if<std::is_base_of<InheritedWidget, T>::value>::type *>
inline Object::Ref<T> BuildContext::dependOnInheritedWidgetOfExactType()
{
    return this->_inherits[typeid(T).name()]->cast<T>();
}

class RuntimeInheritedWidget : public virtual InheritedWidget
{
public:
    RuntimeInheritedWidget(Object::Ref<Widget> child) : InheritedWidget(child) {}
    virtual bool updateShouldNotify(Object::Ref<InheritedWidget> oldWidget) { return false; }
};