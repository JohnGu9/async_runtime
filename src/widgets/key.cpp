#include "framework/widgets/key.h"
#include "framework/elements/element.h"
#include "framework/widgets/widget.h"

void Key::setElement(Object::Ref<Element> element) { this->_element = element; }

Object::Ref<Element> Key::getElement() { return this->_element.lock(); }

Object::Ref<const Widget> Key::getCurrentWidget() { return this->_element.lock()->widget; }

Object::Ref<BuildContext> GlobalKey::getCurrentContext() { return this->getElement(); }

Object::Ref<StatefulWidgetState> GlobalKey::getCurrentState()
{
    Object::Ref<Element> element = this->getElement();
    if (element == nullptr)
        return nullptr;
    if (Object::Ref<StatefulElement> statefulElement = element->cast<StatefulElement>())
        return statefulElement->_state;
    return nullptr;
}