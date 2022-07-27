#pragma once
#include "element.h"

class NamedMultiChildWidget;
class NamedMultiChildElement : public Element
{
public:
    NamedMultiChildElement(ref<NamedMultiChildWidget> widget);
    void attach() override;
    void detach() override;
    void build() override;
    void update(ref<Widget> newWidget) override;
    void notify(ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(ref<Element>)>) override;

protected:
    ref<NamedMultiChildWidget> _namedMultiChildWidget;
    ref<Map<ref<String>, lateref<Element>>> _childrenElements;
};
