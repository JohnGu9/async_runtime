#include "async_runtime/elements/single_child_element.h"
#include "async_runtime/widgets/widget.h"

SingleChildElement::SingleChildElement(ref<Widget> widget) : Element(widget) {}

void SingleChildElement::attachElement(ref<Element> element)
{
    this->_childElement = element;
    this->_childElement->parent = Object::cast<>(this);
    this->_childElement->attach();
}

void SingleChildElement::reattachElement(ref<Element> element)
{
    this->_childElement->detach();
    this->_childElement = element;
    this->_childElement->parent = Object::cast<>(this);
    this->_childElement->attach();
}

void SingleChildElement::detachElement()
{
    this->_childElement->detach();
}