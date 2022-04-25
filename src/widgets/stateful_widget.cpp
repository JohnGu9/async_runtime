#include "async_runtime/elements/stateful_element.h"
#include "async_runtime/widgets/stateful_widget.h"

void State<StatefulWidget>::setState(Function<void()> fn)
{
    assert(this->mounted && "[setState] call on a disposed State");
    fn();
    this->_element->build();
}

ref<Element> StatefulWidget::createElement()
{
    return Object::create<StatefulElement>(self());
}
