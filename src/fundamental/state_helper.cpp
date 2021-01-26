#include "async_runtime/fundamental/state_helper.h"
#include "async_runtime/fundamental/scheduler.h"
#include "async_runtime/widgets/stateful_widget.h"

Object::Ref<BuildContext> StateHelper::getContextfromState(State<StatefulWidget> *state)
{
    assert(state->mounted && "Can not access context from an unmounted state");
    return state->_element.lock();
}

Object::Ref<ThreadPool> StateHelper::getHandlerfromState(State<StatefulWidget> *state)
{
    assert(state->mounted && "Can not access context from an unmounted state");
    return Scheduler::of(getContextfromState(state));
}