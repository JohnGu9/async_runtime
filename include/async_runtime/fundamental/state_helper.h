#pragma once

#include "../widgets/state.h"
#include "../contexts/build_context.h"

class ThreadPool;
class StateHelper
{
protected:
    static ref<BuildContext> getContextfromState(ref<State<StatefulWidget>> state);
    static ref<ThreadPool> getHandlerfromState(ref<State<StatefulWidget>> state);
};
