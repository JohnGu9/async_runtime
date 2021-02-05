#include "async_runtime/fundamental/async.h"
#include "async_runtime/fundamental/dispatcher.h"
#include "async_runtime/fundamental/scheduler.h"
#include "async_runtime/widgets/stateful_widget.h"

Dispatcher::Dispatcher(Object::Ref<ThreadPool> scheduler, Object::Ref<ThreadPool> threadPool, size_t threads)
    : _callbackHandler(scheduler), _threadPool(threadPool)
{
    assert(this->_callbackHandler && "Scheduler is required. ");
    if (this->_threadPool == nullptr && threads > 0)
    {
        this->_ownWorkThread = true;
        this->_threadPool = Object::create<ThreadPool>(threads);
    }
    assert(this->_threadPool);
}

Dispatcher::Dispatcher(State<StatefulWidget> *state, Object::Ref<ThreadPool> threadPool, size_t threads)
    : _threadPool(threadPool)
{
    assert(state && "State is required. ");
    Object::Ref<BuildContext> context = getContextfromState(state);
    this->_callbackHandler = Scheduler::of(context);
    assert(this->_callbackHandler && "Can't find out scheduler in context. ");
    if (this->_threadPool == nullptr && threads > 0)
    {
        this->_ownWorkThread = true;
        this->_threadPool = Object::create<ThreadPool>(threads);
    }
    assert(this->_threadPool);
}

void Dispatcher::post(Function<void()> fn)
{
    this->_threadPool->post(fn.toStdFunction());
}

void Dispatcher::post(Function<void(RunOnMainThread runner)> fn)
{
    Object::Ref<Dispatcher> self = Object::cast<>(this);
    this->_threadPool->post(
        fn.toStdFunction(),
        [self](Function<void()> job) { self->_callbackHandler->post(job.toStdFunction()); });
}

Dispatcher::~Dispatcher()
{
    assert((!this->_ownWorkThread || !this->_threadPool->isActive()) && "Detect memory leak from Dispatcher. Call [dispose]");
}

void Dispatcher::dispose()
{
    if (this->_ownWorkThread)
        this->_threadPool->dispose();
    this->_callbackHandler = nullptr;
}
