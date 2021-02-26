#pragma once

#include "../widgets/state.h"
#include "../contexts/build_context.h"

class ThreadPool;
class StateHelper
{
protected:
    static ref<BuildContext> getContextfromState(State<StatefulWidget> *state);
    static ref<ThreadPool> getHandlerfromState(State<StatefulWidget> *state);
};