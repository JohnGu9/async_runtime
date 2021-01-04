#pragma once

#include "key.h"
#include "../basic/function.h"
#include "../contexts/build_context.h"
#include "../elements/element.h"

class Widget : public virtual Object
{
    Widget() = delete;

public:
    Widget(Object::Ref<Key> key);
    virtual Object::Ref<Key> getKey();
    virtual bool canUpdate(Object::Ref<Widget> other);
    virtual Object::Ref<Element> createElement() = 0;

protected:
    const Object::Ref<Key> _key;
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

class StatefulWidgetState : public virtual Object
{
public:
    StatefulWidgetState();

    // @mustCallSuper
    virtual void initState();
    // @mustCallSuper
    virtual void didWidgetUpdated(Object::Ref<StatefulWidget> oldWidget);
    // @mustCallSuper
    virtual void didDependenceChanged();
    // @mustCallSuper
    virtual void dispose();
    // @immutable
    void setState(Object::Ref<Fn<void()>>);
    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) = 0;

    friend StatefulElement;

protected:
    virtual Object::Ref<BuildContext> getContext();

    bool mounted;
    Object::WeakRef<StatefulElement> element;

private:
    Object::Ref<StatefulElement> getElement();
};

class StatefulWidget : public Widget
{
public:
    StatefulWidget(Object::Ref<Key> key = nullptr);
    virtual Object::Ref<StatefulWidgetState> createState() = 0;

protected:
    virtual Object::Ref<Element> createElement() override;
};

template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type * = nullptr>
class State : public virtual StatefulWidgetState
{
public:
protected:
    virtual Object::Ref<T> getWidget()
    {
        Object::Ref<StatefulElement> element = this->element.lock();
        return element->_statefulWidget->cast<T>();
    }
};

inline Object::Ref<Key> Widget::getKey()
{
    return this->_key;
}

class InheritedWidget : public virtual StatelessWidget, public virtual Inheritance
{
public:
    InheritedWidget(Object::Ref<Widget> child, Object::Ref<Key> key = nullptr);
    virtual bool updateShouldNotify(Object::Ref<InheritedWidget> oldWidget) = 0;
    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) override;
    virtual Object::Ref<Element> createElement() override;

protected:
    Object::Ref<Widget> _child;
};

template <typename T, typename std::enable_if<std::is_base_of<InheritedWidget, T>::value>::type *>
inline Object::Ref<T> BuildContext::dependOnInheritedWidgetOfExactType()
{
    return this->_inheritances[typeid(T).name()]->cast<T>();
}

class NotificationListener : public virtual StatelessWidget
{
public:
    NotificationListener(Object::Ref<Widget> child, Object::Ref<Key> key = nullptr);
    // virtual bool onNotification(Object::Ref<T> oldWidget) = 0;
    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) override;
    virtual Object::Ref<Element> createElement() override;

protected:
    Object::Ref<Widget> _child;
};

class MultiChildWidget : public virtual Widget
{
public:
    MultiChildWidget(Object::List<Object::Ref<Widget>> children, Object::Ref<Key> key = nullptr);
    virtual Object::Ref<Element> createElement();

    friend MultiChildElement;

protected:
    Object::List<Object::Ref<Widget>> _children;
};