#include "async_runtime/widgets/stateful_widget.h"
#include "async_runtime/elements/stateful_element.h"

void State<StatefulWidget>::setState(Function<void()> fn)
{
    assert(this->mounted && "[setState] call on a disposed State");
    assert(this->_element->_lifeCycle == StatefulElement::LifeCycle::mounted && "[setState] can't not call in widget lifecycle hooker [build]. ");
    fn();
    this->_element->build();
}

ref<Element> StatefulWidget::createElement()
{
    return Object::create<StatefulElement>(self());
}
