#pragma once

#include "../contexts/build_context.h"

class Widget;
class LeafWidget;
class InheritedWidget;

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
};

class LeafElement : public virtual Element
{
public:
    LeafElement(Object::Ref<LeafWidget> widget);
    virtual void build() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
    virtual void notify() override;
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

protected:
    Object::Ref<Widget> _child;
    Object::Ref<Element> _childElement;
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
};