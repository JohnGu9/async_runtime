#include "async_runtime/widgets/stateful_widget.h"
#include "async_runtime/elements/stateful_element.h"

State<StatefulWidget>::State() : _loop(EventLoopGetterMixin::ensureEventLoop(nullptr)) {}

State<StatefulWidget>::~State() { DEBUG_ASSERT(_mounted == false && "[dispose] must to call super::dispose"); }

void State<StatefulWidget>::initState()
{
    DEBUG_ASSERT(_mounted == false && "This [State] class mount twice is not allowed. User should not reuse [State] class or manually call [initState]");
    _mounted = true;
}

void State<StatefulWidget>::dispose()
{
    DEBUG_ASSERT(_mounted == true && "This [State] class dispose more then twice is not allowed. User should not reuse [State] class or manually call [dispose]");
    _mounted = false;
    static finalref<List<Function<void()>>> dummy = List<Function<void()>>::create();
}

void State<StatefulWidget>::setState(Function<void()> fn)
{
    DEBUG_ASSERT(this->_loop == EventLoop::runningEventLoop && "[setState] can't call in other event loops");
    DEBUG_ASSERT(this->_mounted && "[setState] call on a disposed State");
    DEBUG_ASSERT(this->_element->_lifeCycle == StatefulElement::LifeCycle::mounted && "[setState] can't not call in widget lifecycle hooker. ");
    this->_element->scheduleRebuild(fn, this->_loop);
}

ref<EventLoop> State<StatefulWidget>::eventLoop() { return this->_loop; }

ref<Element> StatefulWidget::createElement()
{
    return Object::create<StatefulElement>(self());
}
