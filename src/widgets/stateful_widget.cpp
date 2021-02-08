#include "async_runtime/widgets/stateful_widget.h"
#include "async_runtime/fundamental/scheduler.h"

void State<StatefulWidget>::setState(Function<void()> fn)
{
    assert(Scheduler::of(context)->childrenThreadName(0) // scheduler threadpool only have one thread that id is zero(0)
               == ThreadPool::thisThreadName &&
           "Update state should call on state's main thread");
    fn();
    this->_element->build();
}

Object::Ref<Element> StatefulWidget::createElement()
{
    return Object::create<StatefulElement>(Object::cast<>(this));
}
