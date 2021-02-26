#include "async_runtime/elements/single_child_element.h"
#include "async_runtime/widgets/widget.h"

SingleChildElement::SingleChildElement(ref<Widget> widget) : Element(widget) {}

void SingleChildElement::attachElement(ref<Element> element)
{
    assert(element);
    this->_childElement = element;
    this->_childElement->parent = Object::cast<>(this);
    this->_childElement->attach();
}

void SingleChildElement::reattachElement(ref<Element> element)
{
    assert(element != nullptr && "Widget [createElement] return null isn't allowed. ");
    assert(this->_childElement && "Child element already detach from build tree. System callback invoke by mistake. ");
    this->_childElement->detach();
    this->_childElement = element;
    this->_childElement->parent = Object::cast<>(this);
    this->_childElement->attach();
}

void SingleChildElement::detachElement()
{
    assert(this->_childElement != nullptr && "Child element already detach from build tree. System callback invoke by mistake. ");
    this->_childElement->detach();
    this->_childElement = nullptr;
}