#pragma once

#include "../object.h"
#include "state_helper.h"
#include "disposable.h"

class ThreadPool;
class Dispatcher : public virtual Object, public virtual StateHelper, public virtual Disposable
{
public:
    using RunOnMainThread = Function<void(Function<void()> job)>;
    Dispatcher(Object::Ref<ThreadPool> handler, Object::Ref<ThreadPool> threadPool = nullptr, size_t threads = 1);
    Dispatcher(State<StatefulWidget> *state, Object::Ref<ThreadPool> threadPool = nullptr, size_t threads = 1);
    virtual ~Dispatcher();
    void dispose() override;

protected:
    virtual void post(Function<void()>);
    virtual void post(Function<void(RunOnMainThread runner)>);

private:
    Object::Ref<ThreadPool> _callbackHandler; // main thread for callback
    Object::Ref<ThreadPool> _threadPool;      // work thread

    bool _ownWorkThread = false;
};
