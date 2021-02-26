#pragma once

#include "element.h"

class LeafWidget;
class LeafElement : public Element
{
public:
    LeafElement(ref<LeafWidget> widget);
    void build() override;
    void update(ref<Widget> newWidget) override;
    void notify(ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(ref<Element>)>) override;
};