#include "async_runtime/widgets/key.h"
#include "async_runtime/elements/stateful_element.h"
#include "async_runtime/widgets/widget.h"

void GlobalKey::setElement(ref<Element> element)
{
    assert(this->_element.toOption() == nullptr && "One [GlobalKey] instance can't mount more than one element. ");
    this->_element = element;
}

void GlobalKey::dispose()
{
    this->_element = nullptr;
}

option<Widget> GlobalKey::getCurrentWidget()
{
    lateref<Element> element;
    if (this->_element.isNotNull(element))
        return element->getWidget();
    return nullptr;
}

option<BuildContext> GlobalKey::getCurrentContext() { return this->_element.toOption(); }

option<State<StatefulWidget>> GlobalKey::getCurrentState()
{
    lateref<Element> element;
    if (this->_element.isNotNull(element))
    {
        lateref<StatefulElement> statefulElement;
        if (element->cast<StatefulElement>().isNotNull(statefulElement))
            return statefulElement->_state;
    }
    return nullptr;
}
