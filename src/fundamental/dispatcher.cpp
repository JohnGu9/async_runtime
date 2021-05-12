#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/dispatcher.h"
#include "async_runtime/fundamental/scheduler.h"

Dispatcher::Dispatcher(ref<ThreadPool> handler) : _callbackHandler(handler) {}

Dispatcher::Dispatcher(ref<State<StatefulWidget>> state) : _callbackHandler(getHandlerfromState(state)) {}

void Dispatcher::dispose() {}

std::future<void> Dispatcher::run(Function<void()> fn) { return this->_callbackHandler->post(fn.toStdFunction()); }

std::future<void> Dispatcher::microTask(Function<void()> fn) { return this->_callbackHandler->microTask(fn.toStdFunction()); }

AsyncDispatcher::AsyncDispatcher(ref<ThreadPool> handler, option<ThreadPool> threadPool, size_t threads = 1)
    : Dispatcher(handler),
      _ownThreadPool(threadPool != nullptr ? option<ThreadPool>::null() : Object::create<ThreadPool>(threads))
{
    _threadPool = threadPool.isNotNullElse([this] { return this->_ownThreadPool.assertNotNull(); });
}

AsyncDispatcher::AsyncDispatcher(ref<State<StatefulWidget>> state, option<ThreadPool> threadPool, size_t threads = 1)
    : Dispatcher(state),
      _ownThreadPool(threadPool != nullptr ? option<ThreadPool>::null() : Object::create<ThreadPool>(threads))
{
    _threadPool = threadPool.isNotNullElse([this] { return this->_ownThreadPool.assertNotNull(); });
}

std::future<void> AsyncDispatcher::post(Function<void()> fn) { return this->_threadPool->post(fn.toStdFunction()); }

std::future<void> AsyncDispatcher::post(Function<void(RunOnMainThread runner)> fn)
{
    ref<AsyncDispatcher> self = self();
    return this->_threadPool->post(
        fn.toStdFunction(),
        [=](Function<void()> job) { self->run(job); });
}

AsyncDispatcher::~AsyncDispatcher()
{
    assert((this->_ownThreadPool == nullptr || !this->_ownThreadPool.assertNotNull()->isActive()) && "Detect memory leak from Dispatcher. Call [dispose]");
}

void AsyncDispatcher::dispose()
{
    lateref<ThreadPool> ownThreadPool;
    if (this->_ownThreadPool.isNotNull(ownThreadPool))
        ownThreadPool->dispose();
    Dispatcher::dispose();
}
