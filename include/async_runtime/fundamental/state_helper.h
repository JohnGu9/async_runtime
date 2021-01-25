#pragma once

#include "../basic/state.h"
#include "../contexts/build_context.h"

class ThreadPool;
class StateHelper
{
protected:
    static Object::Ref<BuildContext> getContextfromState(State<StatefulWidget> *state);
    static Object::Ref<ThreadPool> getHandlerfromState(State<StatefulWidget> *state);
};