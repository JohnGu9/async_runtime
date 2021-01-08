#include "framework/widgets/key.h"

void Key::setElement(Object::Ref<Element> element)
{
    this->_element = element;
}

Object::Ref<Element> Key::getElement()
{
    return this->_element.lock();
}

Object::Ref<Widget> Key::getCurrentWidget()
{
    return this->_element.lock()->widget;
}

Object::Ref<BuildContext> GlobalKey::getCurrentContext()
{
    return this->getElement();
}

Object::Ref<StatefulWidgetState> GlobalKey::getCurrentState()
{
    if (Object::Ref<StatefulElement> element = this->getElement()->cast<StatefulElement>())
        return element->_state;
    return nullptr;
}