#pragma once

#include "scheduler.h"
#include "../widgets/stateful_widget.h"

class Dispatcher : public Object
{
public:
    Dispatcher(Scheduler::Handler handler, Object::Ref<ThreadPool> threadPool = nullptr, size_t threads = 1);
    Dispatcher(State<StatefulWidget> *state, Object::Ref<ThreadPool> threadPool = nullptr, size_t threads = 1);
    virtual ~Dispatcher();
    virtual void dispose();

protected:
    Scheduler::Handler _callbackHandler;         // main thread for callback
    Object::Ref<ThreadPool> _threadPool; // work thread

    bool _ownWorkThread = false;
};