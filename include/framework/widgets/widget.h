#pragma once

#include "key.h"
#include "../basic/function.h"
#include "../basic/notification.h"
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

class LeafWidget : public Widget
{
public:
    static Object::Ref<LeafWidget> &factory();
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

class StatefulElement;
class Dispatcher;
class StatefulWidgetState : public Object
{
    friend StatefulElement;
    friend Dispatcher;

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
    void setState(Fn<void()>);
    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) = 0;

protected:
    virtual Object::Ref<BuildContext> getContext() const;
    virtual bool getMounted() const;

    Object::WeakRef<StatefulElement> element;

private:
    bool mounted;
    Object::Ref<StatefulElement> getElement() const;
};

class StatefulWidget : public Widget
{
public:
    using State = StatefulWidgetState;
    StatefulWidget(Object::Ref<Key> key = nullptr);
    virtual Object::Ref<State> createState() = 0;

protected:
    virtual Object::Ref<Element> createElement() override;
};

template <typename T, typename std::enable_if<std::is_base_of<StatefulWidget, T>::value>::type * = nullptr>
class State : public virtual StatefulWidgetState
{
public:
protected:
    virtual Object::Ref<const T> getWidget()
    {
        Object::Ref<StatefulElement> element = this->element.lock();
        assert(element);
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
    auto iter = this->_inheritances.find(typeid(T).name());
    if (iter == this->_inheritances.end())
        return nullptr;
    return iter->second->cast<T>();
}

class NotificationListener : public StatelessWidget
{
public:
    NotificationListener(Object::Ref<Widget> child, Fn<bool(Object::Ref<Notification> notification)> onNotification, Object::Ref<Key> key = nullptr);
    virtual bool onNotification(Object::Ref<Notification> notification) { return this->_onNotification(notification); }
    virtual Object::Ref<Widget> build(Object::Ref<BuildContext> context) override;
    virtual Object::Ref<Element> createElement() override;

protected:
    Object::Ref<Widget> _child;
    Fn<bool(Object::Ref<Notification> notification)> _onNotification;
};

class MultiChildWidget : public Widget
{
public:
    MultiChildWidget(const Object::List<Object::Ref<Widget>> &children, Object::Ref<Key> key = nullptr);
    virtual Object::Ref<Element> createElement();

    friend MultiChildElement;

protected:
    Object::List<Object::Ref<Widget>> _children;
};

