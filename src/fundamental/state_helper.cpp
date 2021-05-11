#include "async_runtime/fundamental/state_helper.h"
#include "async_runtime/fundamental/scheduler.h"
#include "async_runtime/widgets/stateful_widget.h"

ref<BuildContext> StateHelper::getContextfromState(ref<State<StatefulWidget>> state)
{
    assert(state->_mounted && "Can not access context from an unmounted state");
    return state->_element;
}

ref<ThreadPool> StateHelper::getHandlerfromState(ref<State<StatefulWidget>> state)
{
    assert(state->_mounted && "Can not access context from an unmounted state");
    return Scheduler::of(getContextfromState(state));
}