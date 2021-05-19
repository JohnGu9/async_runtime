#pragma once

#include "../contexts/build_context.h"

class Widget;
class Element : public BuildContext
{
public:
    Element(ref<Widget> widget);
    weakref<Element> parent;

    virtual void attach();
    virtual void detach();
    virtual void build() = 0;

    virtual void update(ref<Widget> newWidget);
    virtual void notify(ref<Widget> newWidget);

    virtual void visitDescendant(Function<bool(ref<Element>)>) = 0;
    virtual void visitAncestor(Function<bool(ref<Element>)>);

    virtual const ref<Widget> &getWidget() const { return widget; }
};
