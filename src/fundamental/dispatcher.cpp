#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/dispatcher.h"

Dispatcher::Dispatcher(ref<ThreadPool> handler) : _callbackHandler(handler) {}

Dispatcher::Dispatcher(ref<State<StatefulWidget>> state) : _callbackHandler(getHandlerfromState(state)) {}

void Dispatcher::dispose() { Object::detach(_callbackHandler); }

std::future<void> Dispatcher::postToMainThread(Function<void()> fn) { return this->_callbackHandler->post(fn.toStdFunction()); }

std::future<void> Dispatcher::microTaskToMainThread(Function<void()> fn) { return this->_callbackHandler->microTask(fn.toStdFunction()); }

static option<ThreadPool> _getThreadPool(option<ThreadPool> threadPool, size_t threads)
{
    if (threadPool == nullptr)
        return Object::create<ThreadPool>(threads);
    else
        return nullptr;
}

AsyncDispatcher::AsyncDispatcher(ref<ThreadPool> handler, option<ThreadPool> threadPool, size_t threads = 1)
    : Dispatcher(handler), _ownThreadPool(_getThreadPool(threadPool, 1))
{
    _threadPool = threadPool.isNotNullElse([this] { return this->_ownThreadPool.assertNotNull(); });
}

AsyncDispatcher::AsyncDispatcher(ref<State<StatefulWidget>> state, option<ThreadPool> threadPool, size_t threads = 1)
    : Dispatcher(state), _ownThreadPool(_getThreadPool(threadPool, 1))
{
    _threadPool = threadPool.isNotNullElse([this] { return this->_ownThreadPool.assertNotNull(); });
}

std::future<void> AsyncDispatcher::post(Function<void()> fn) { return this->_threadPool->post(fn.toStdFunction()); }

std::future<void> AsyncDispatcher::post(Function<void(RunOnMainThread runner)> fn)
{
    ref<AsyncDispatcher> self = self();
    return this->_threadPool->post(
        fn.toStdFunction(),
        [=](Function<void()> job) { self->postToMainThread(job); });
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
