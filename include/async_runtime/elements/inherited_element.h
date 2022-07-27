#pragma once
#include "single_child_element.h"

class InheritedWidget;
class InheritedElement : public SingleChildElement
{
public:
    InheritedElement(ref<InheritedWidget> child);
    ref<InheritedWidget> _inheritWidget;

    void attach() override;
    void detach() override;
    void build() override;
    void update(ref<Widget> newWidget) override;
    void notify(ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(ref<Element>)>) override;
};
