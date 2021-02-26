#pragma once

#include "../object.h"
#include "state_helper.h"
#include "disposable.h"

class ThreadPool;
class Dispatcher : public virtual Object, public StateHelper, public Disposable
{
public:
    Dispatcher(ref<ThreadPool> handler);
    Dispatcher(State<StatefulWidget> *state);
    void dispose() override;

protected:
    virtual void run(Function<void()>);
    virtual void microTask(Function<void()>);

private:
    ref<ThreadPool> _callbackHandler; // main thread for callback
};

class AsyncDispatcher : public Dispatcher
{
public:
    using RunOnMainThread = Function<void(Function<void()> job)>;
    AsyncDispatcher(ref<ThreadPool> handler, ref<ThreadPool>, size_t threads);
    AsyncDispatcher(State<StatefulWidget> *state, ref<ThreadPool>, size_t threads);
    virtual ~AsyncDispatcher();
    void dispose() override;

protected:
    virtual void post(Function<void()>);
    virtual void post(Function<void(RunOnMainThread runner)>);

private:
    ref<ThreadPool> _ownThreadPool;
    ref<ThreadPool> _threadPool; // work thread
};
