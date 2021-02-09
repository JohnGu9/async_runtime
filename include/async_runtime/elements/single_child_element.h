#pragma once

#include "element.h"

class SingleChildElement : public Element
{
public:
    SingleChildElement(Object::Ref<Widget> widget);

protected:
    Object::Ref<Element> _childElement;
    void attachElement(Object::Ref<Element>);
    void reattachElement(Object::Ref<Element>);
    void detachElement();
};