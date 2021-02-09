#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/dispatcher.h"
#include "async_runtime/fundamental/scheduler.h"
#include "async_runtime/widgets/stateful_widget.h"

Dispatcher::Dispatcher(Object::Ref<ThreadPool> handler) : _callbackHandler(handler) {}

Dispatcher::Dispatcher(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)) {}

void Dispatcher::dispose() {}

void Dispatcher::run(Function<void()> fn) { this->_callbackHandler->post(fn.toStdFunction()); }

void Dispatcher::microTask(Function<void()> fn) { this->_callbackHandler->microTask(fn.toStdFunction()); }

AsyncDispatcher::AsyncDispatcher(Object::Ref<ThreadPool> handler, Object::Ref<ThreadPool> threadPool, size_t threads = 1)
    : Dispatcher(handler),
      _ownThreadPool(threadPool != nullptr ? nullptr : Object::create<ThreadPool>(threads)),
      _threadPool(threadPool != nullptr ? threadPool : _ownThreadPool) { assert(this->_threadPool); }

AsyncDispatcher::AsyncDispatcher(State<StatefulWidget> *state, Object::Ref<ThreadPool> threadPool, size_t threads = 1)
    : Dispatcher(state),
      _ownThreadPool(threadPool != nullptr ? nullptr : Object::create<ThreadPool>(threads)),
      _threadPool(threadPool != nullptr ? threadPool : _ownThreadPool) { assert(this->_threadPool); }

void AsyncDispatcher::post(Function<void()> fn) { this->_threadPool->post(fn.toStdFunction()); }

void AsyncDispatcher::post(Function<void(RunOnMainThread runner)> fn)
{
    Object::Ref<AsyncDispatcher> self = Object::cast<>(this);
    this->_threadPool->post(
        fn.toStdFunction(),
        [self](Function<void()> job) { self->run(job); });
}

AsyncDispatcher::~AsyncDispatcher()
{
    assert((this->_ownThreadPool == nullptr || this->_ownThreadPool->isActive()) && "Detect memory leak from Dispatcher. Call [dispose]");
}

void AsyncDispatcher::dispose()
{
    if (this->_ownThreadPool != nullptr)
        this->_ownThreadPool->dispose();
    Dispatcher::dispose();
}
