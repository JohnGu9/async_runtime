#include "async_runtime/fundamental/state_helper.h"
#include "async_runtime/fundamental/scheduler.h"
#include "async_runtime/widgets/stateful_widget.h"

Object::Ref<BuildContext> StateHelper::getContextfromState(State<StatefulWidget> *state)
{
    return state->_element.lock();
}

Object::Ref<ThreadPool> StateHelper::getHandlerfromState(State<StatefulWidget> *state)
{
    return Scheduler::of(getContextfromState(state));
}