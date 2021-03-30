#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/dispatcher.h"
#include "async_runtime/fundamental/scheduler.h"
#include "async_runtime/widgets/stateful_widget.h"

Dispatcher::Dispatcher(ref<ThreadPool> handler) : _callbackHandler(handler) {}

Dispatcher::Dispatcher(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)) {}

void Dispatcher::dispose() {}

void Dispatcher::run(Function<void()> fn) { this->_callbackHandler->post(fn.toStdFunction()); }

void Dispatcher::microTask(Function<void()> fn) { this->_callbackHandler->microTask(fn.toStdFunction()); }

AsyncDispatcher::AsyncDispatcher(ref<ThreadPool> handler, option<ThreadPool> threadPool, size_t threads = 1)
    : Dispatcher(handler),
      _ownThreadPool(threadPool != nullptr ? option<ThreadPool>::null() : Object::create<ThreadPool>(threads))
{
    _threadPool = threadPool.isNotNullElse([this] { return this->_ownThreadPool.assertNotNull(); });
}

AsyncDispatcher::AsyncDispatcher(State<StatefulWidget> *state, option<ThreadPool> threadPool, size_t threads = 1)
    : Dispatcher(state),
      _ownThreadPool(threadPool != nullptr ? option<ThreadPool>::null() : Object::create<ThreadPool>(threads))
{
    _threadPool = threadPool.isNotNullElse([this] { return this->_ownThreadPool.assertNotNull(); });
}

void AsyncDispatcher::post(Function<void()> fn) { this->_threadPool->post(fn.toStdFunction()); }

void AsyncDispatcher::post(Function<void(RunOnMainThread runner)> fn)
{
    ref<AsyncDispatcher> self = self();
    this->_threadPool->post(
        fn.toStdFunction(),
        [self](Function<void()> job) { self->run(job); });
}

AsyncDispatcher::~AsyncDispatcher()
{
    assert((this->_ownThreadPool == nullptr || this->_ownThreadPool.assertNotNull()->isActive()) && "Detect memory leak from Dispatcher. Call [dispose]");
}

void AsyncDispatcher::dispose()
{
    lateref<ThreadPool> ownThreadPool;
    if (this->_ownThreadPool.isNotNull(ownThreadPool))
        ownThreadPool->dispose();
    Dispatcher::dispose();
}
