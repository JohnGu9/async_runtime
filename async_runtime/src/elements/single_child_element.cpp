#include "framework/elements/element.h"
#include "framework/widgets/widget.h"

SingleChildElement::SingleChildElement(Object::Ref<Widget> widget) : Element(widget) {}

void SingleChildElement::attachElement(Object::Ref<Element> element)
{
    assert(element);
    this->_childElement = element;
    this->_childElement->parent = Object::cast<Element>(this);
    this->_childElement->attach();
}

void SingleChildElement::reattachElement(Object::Ref<Element> element)
{
    assert(element != nullptr && "Widget [createElement] return null isn't allowed. ");
    this->_childElement->detach();
    this->_childElement = element;
    this->_childElement->parent = Object::cast<Element>(this);
    this->_childElement->attach();
}

void SingleChildElement::detachElement()
{
    assert(this->_childElement != nullptr);
    this->_childElement->detach();
    this->_childElement = nullptr;
}