#pragma once

#include "../object.h"
#include "../basic/state.h"
#include "state_helper.h"

class ThreadPool;
class Dispatcher : public Object, public StateHelper
{
public:
    Dispatcher(Object::Ref<ThreadPool> handler, Object::Ref<ThreadPool> threadPool = nullptr, size_t threads = 1);
    Dispatcher(State<StatefulWidget> *state, Object::Ref<ThreadPool> threadPool = nullptr, size_t threads = 1);
    virtual ~Dispatcher();
    virtual void dispose();

protected:
    Object::Ref<ThreadPool> _callbackHandler; // main thread for callback
    Object::Ref<ThreadPool> _threadPool;      // work thread

    bool _ownWorkThread = false;
};