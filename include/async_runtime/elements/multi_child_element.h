#pragma once

#include "element.h"

class MultiChildWidget;
class MultiChildElement : public Element
{
public:
    MultiChildElement(Object::Ref<MultiChildWidget> widget);
    void attach() override;
    void detach() override;
    void build() override;
    void update(Object::Ref<Widget> newWidget) override;
    void notify(Object::Ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(Object::Ref<Element>)>) override;

protected:
    Object::Ref<MultiChildWidget> _multiChildWidget;
    List<Object::Ref<Element> > _childrenElements;
};
