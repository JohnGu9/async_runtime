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
    virtual std::future<void> run(Function<void()>);
    virtual std::future<void> microTask(Function<void()>);

    ref<ThreadPool> _callbackHandler; // main thread for callback
};

class AsyncDispatcher : public Dispatcher
{
public:
    using RunOnMainThread = Function<void(Function<void()> job)>;
    AsyncDispatcher(ref<ThreadPool> handler, option<ThreadPool>, size_t threads);
    AsyncDispatcher(State<StatefulWidget> *state, option<ThreadPool>, size_t threads);
    virtual ~AsyncDispatcher();
    void dispose() override;

protected:
    virtual std::future<void> post(Function<void()>);
    virtual std::future<void> post(Function<void(RunOnMainThread runner)>);

    option<ThreadPool> _ownThreadPool;
    lateref<ThreadPool> _threadPool; // work thread
};
