#pragma once

#include "single_child_element.h"

class InheritedWidget;
class InheritedElement : public SingleChildElement
{
public:
    InheritedElement(Object::Ref<InheritedWidget> child);
    Object::Ref<InheritedWidget> _inheritWidget;

    void attach() override;
    void detach() override;
    void build() override;
    void update(Object::Ref<Widget> newWidget) override;
    void notify(Object::Ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(Object::Ref<Element>)>) override;
};