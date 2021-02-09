#pragma once

#include "../basic/function.h"
#include "../widgets/state.h"
#include "../contexts/build_context.h"
#include "../fundamental/async.h"

class Widget;
class Element : public BuildContext
{
public:
    Element(Object::Ref<Widget> widget);
    Object::WeakRef<Element> parent;

    virtual void attach();
    virtual void detach();
    virtual void build() = 0;

    virtual void update(Object::Ref<Widget> newWidget);
    virtual void notify(Object::Ref<Widget> newWidget);

    virtual void visitDescendant(Function<bool(Object::Ref<Element>)>) = 0;
    virtual void visitAncestor(Function<bool(Object::Ref<Element>)>);
};
