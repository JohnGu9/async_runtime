#pragma once

#include "../basic/function.h"
#include "../contexts/build_context.h"
#include "../basic/observable.h"

class Widget;
class LeafWidget;
class StatelessWidget;
class StatefulWidget;
class StatefulWidgetState;

class InheritedWidget;
class NotificationListener;
class MultiChildWidget;

class Element : public virtual BuildContext
{
public:
    Element(Object::Ref<Widget> widget);
    Object::WeakRef<Element> parent;
    Object::Ref<Widget> widget;

    virtual void build() = 0;
    virtual void update(Object::Ref<Widget> newWidget) = 0;
    virtual void notify();
    virtual void attach();
    virtual void detach();

    virtual void visitDescendant(Fn<void(Object::Ref<Element>)>) = 0;
    virtual void visitAncestor(Fn<void(Object::Ref<Element>)>);
};

class LeafElement : public virtual Element
{
public:
    LeafElement(Object::Ref<LeafWidget> widget);
    virtual void build() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
    virtual void notify() override;

    virtual void visitDescendant(Fn<void(Object::Ref<Element>)>) override;
};

class RootElement : public virtual Element
{
public:
    RootElement(Object::Ref<Widget> child);

    virtual void build() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
    virtual void notify() override;
    virtual void attach() override;
    virtual void detach() override;

    virtual void visitDescendant(Fn<void(Object::Ref<Element>)>) override;
    virtual void visitAncestor(Fn<void(Object::Ref<Element>)>) override;

protected:
    Object::Ref<Widget> _child;
    Object::Ref<Element> _childElement;
};

class StatelessElement : public virtual Element
{
public:
    StatelessElement(Object::Ref<StatelessWidget> widget);

protected:
    Object::Ref<StatelessWidget> _statelessWidget;
    Object::Ref<Element> _childElement;

    virtual void build() override;
    virtual void notify() override;
    virtual void attach() override;
    virtual void detach() override;
    virtual void update(Object::Ref<Widget> newWidget) override;

    virtual void visitDescendant(Fn<void(Object::Ref<Element>)>) override;
};

class StatefulElement : public virtual Element
{
    friend StatefulWidgetState;

public:
    StatefulElement(Object::Ref<StatefulWidget> widget);

    Object::Ref<StatefulWidget> _statefulWidget;
    Object::Ref<StatefulWidgetState> _state;

protected:
    class LifeCycle
    {
    public:
        enum Enum
        {
            uninitialized,
            initialized,
            mounted,
            building,
            unmount,
        };
    };
    Object::Ref<Element> _childElement;
    LifeCycle::Enum _lifeCycle;

    virtual void build() override;
    virtual void notify() override;
    virtual void attach() override;
    virtual void detach() override;
    virtual void update(Object::Ref<Widget> newWidget) override;

    virtual void visitDescendant(Fn<void(Object::Ref<Element>)>) override;
};

class InheritedElement : public virtual Element
{
public:
    InheritedElement(Object::Ref<InheritedWidget> child);
    Object::Ref<InheritedWidget> _inheritWidget;
    Object::Ref<Element> _childElement;

    virtual void build() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
    virtual void notify() override;
    virtual void attach() override;
    virtual void detach() override;

    virtual void visitDescendant(Fn<void(Object::Ref<Element>)>) override;
};

class NotificationListenerElement : public virtual StatelessElement
{
public:
    NotificationListenerElement(Object::Ref<NotificationListener> child);
    Object::Ref<NotificationListener> _notificationListenerWidget;

    virtual void update(Object::Ref<Widget> newWidget) override;
    virtual void detach() override;
};

class MultiChildElement : public virtual Element
{
public:
    MultiChildElement(Object::Ref<MultiChildWidget> widget);
    virtual void build() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
    virtual void notify() override;
    virtual void attach() override;
    virtual void detach() override;

    virtual void visitDescendant(Fn<void(Object::Ref<Element>)>) override;

protected:
    Object::Ref<MultiChildWidget> _multiChildWidget;
    Object::List<Object::Ref<Element>> _childrenElements;
};