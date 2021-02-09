#pragma once

#include "element.h"

class NamedMultiChildWidget;
class NamedMultiChildElement : public Element
{
public:
    NamedMultiChildElement(Object::Ref<NamedMultiChildWidget> widget);
    void attach() override;
    void detach() override;
    void build() override;
    void update(Object::Ref<Widget> newWidget) override;
    void notify(Object::Ref<Widget> newWidget) override;

    void visitDescendant(Function<bool(Object::Ref<Element>)>) override;

protected:
    Object::Ref<NamedMultiChildWidget> _namedMultiChildWidget;
    Map<String, Object::Ref<Element> > _childrenElements;
};
