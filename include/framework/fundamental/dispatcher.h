#pragma once

#include "scheduler.h"
#include "../widgets/widget.h"

class Dispatcher : public Object
{
public:
    Dispatcher(Scheduler::Handler handler, Object::Ref<ThreadPool> threadPool = nullptr, size_t threads = 1);
    Dispatcher(StatefulWidget::State *state, Object::Ref<ThreadPool> threadPool = nullptr, size_t threads = 1);
    virtual ~Dispatcher();
    virtual void dispose();

protected:
    Scheduler::Handler _handler;         // main thread for callback
    Object::Ref<ThreadPool> _threadPool; // work thread

    bool _ownWorkThread = false;
};