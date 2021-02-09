#pragma once

#include "../object.h"
#include "state_helper.h"
#include "disposable.h"

class ThreadPool;
class Dispatcher : public virtual Object, public StateHelper, public Disposable
{
public:
    Dispatcher(Object::Ref<ThreadPool> handler);
    Dispatcher(State<StatefulWidget> *state);
    void dispose() override;

protected:
    virtual void run(Function<void()>);

private:
    Object::Ref<ThreadPool> _callbackHandler; // main thread for callback
};

class AsyncDispatcher : public Dispatcher
{
public:
    using RunOnMainThread = Function<void(Function<void()> job)>;
    AsyncDispatcher(Object::Ref<ThreadPool> handler, Object::Ref<ThreadPool>, size_t threads);
    AsyncDispatcher(State<StatefulWidget> *state, Object::Ref<ThreadPool>, size_t threads);
    virtual ~AsyncDispatcher();
    void dispose() override;

protected:
    virtual void post(Function<void()>);
    virtual void post(Function<void(RunOnMainThread runner)>);

private:
    Object::Ref<ThreadPool> _ownThreadPool;
    Object::Ref<ThreadPool> _threadPool; // work thread
};
