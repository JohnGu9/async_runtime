#include "async_runtime/elements/key.h"
#include "async_runtime/elements/stateful_element.h"
#include "async_runtime/widgets/widget.h"

void Key::setElement(option<Element> element)
{
    assert(!this->_element.lock() && "One key can't mount more than one element. ");
    this->_element = element;
}

void Key::dispose()
{
    this->_element = option<Element>(nullptr);
}

option<Element> Key::getElement() { return this->_element.lock(); }

option<const Widget> Key::getCurrentWidget() { return this->_element.lock()->widget; }

option<BuildContext> GlobalKey::getCurrentContext() { return this->getElement(); }

option<State<StatefulWidget>> GlobalKey::getCurrentState()
{
    lateref<Element> element;
    if (this->getElement().isNotNull(element))
    {
        lateref<StatefulElement> statefulElement;
        if (element->cast<StatefulElement>().isNotNull(statefulElement))
            return statefulElement->_state;
    }
    return nullptr;
}