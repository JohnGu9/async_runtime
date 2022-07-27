#pragma once
#include "element.h"

class MultiChildWidget;
class MultiChildElement : public Element
{
public:
    MultiChildElement(ref<MultiChildWidget> widget);
    void attach() override;
    void detach() override;
    void build() override;
    void update(ref<Widget> newWidget) override;
    void notify(ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(ref<Element>)>) override;

protected:
    ref<MultiChildWidget> _multiChildWidget;
    ref<List<ref<Element>>> _childrenElements;
};
