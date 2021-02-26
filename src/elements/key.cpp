#include "async_runtime/elements/key.h"
#include "async_runtime/elements/stateful_element.h"
#include "async_runtime/widgets/widget.h"

void Key::setElement(ref<Element> element)
{
    assert(!this->_element.lock() && "One key can't mount more than one element. ");
    this->_element = element;
}

void Key::dispose()
{
    this->_element = ref<Element>(nullptr);
}

ref<Element> Key::getElement() { return this->_element.lock(); }

ref<const Widget> Key::getCurrentWidget() { return this->_element.lock()->widget; }

ref<BuildContext> GlobalKey::getCurrentContext() { return this->getElement(); }

ref<State<StatefulWidget>> GlobalKey::getCurrentState()
{
    if (ref<Element> element = this->getElement())
        if (ref<StatefulElement> statefulElement = element->cast<StatefulElement>())
            return statefulElement->_state;
    return nullptr;
}