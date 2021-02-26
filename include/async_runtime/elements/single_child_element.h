#pragma once

#include "element.h"

class SingleChildElement : public Element
{
public:
    SingleChildElement(ref<Widget> widget);

protected:
    ref<Element> _childElement;
    void attachElement(ref<Element>);
    void reattachElement(ref<Element>);
    void detachElement();
};