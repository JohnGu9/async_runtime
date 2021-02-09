#pragma once

#include "element.h"

class LeafWidget;
class LeafElement : public Element
{
public:
    LeafElement(Object::Ref<LeafWidget> widget);
    void build() override;
    void update(Object::Ref<Widget> newWidget) override;
    void notify(Object::Ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(Object::Ref<Element>)>) override;
};