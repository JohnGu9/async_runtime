#pragma once

#include "../contexts/build_context.h"

class Widget;
class LeafWidget;

class Element : public virtual BuildContext
{
public:
    Element(Object::Ref<Widget> widget);
    Object::WeakRef<Element> parent;
    Object::Ref<Widget> widget;

    virtual void build() = 0;
    virtual void update(Object::Ref<Widget> newWidget) = 0;
    virtual void attach();
    virtual void detach();
};

class LeafElement : public virtual Element
{
public:
    LeafElement(Object::Ref<LeafWidget> widget);
    virtual void build() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
};

class RootElement : public virtual Element
{
public:
    RootElement(Object::Ref<Widget> widget);
    Object::Ref<Widget> childWidget;
    Object::Ref<Element> _childElement;

    virtual void build() override;
    virtual void update(Object::Ref<Widget> newWidget) override;
    virtual void attach() override;
    virtual void detach() override;
};
