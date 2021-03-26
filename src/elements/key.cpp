#include "async_runtime/elements/key.h"
#include "async_runtime/elements/stateful_element.h"
#include "async_runtime/widgets/widget.h"

void Key::setElement(ref<Element> element)
{
    assert(this->_element.toOption() == nullptr && "One key can't mount more than one element. ");
    this->_element = element;
}

void Key::dispose()
{
    this->_element = nullptr;
}

option<Element> Key::getElement() { return this->_element.toOption(); }

option<const Widget> Key::getCurrentWidget()
{
    lateref<Element> element;
    if (this->_element.isNotNull(element))
        return element->widget;
    return nullptr;
}

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