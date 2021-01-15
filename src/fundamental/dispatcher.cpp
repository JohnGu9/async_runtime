#include "framework/fundamental/dispatcher.h"

Dispatcher::Dispatcher(Object::Ref<ThreadPool> scheduler, Object::Ref<ThreadPool> threadPool, size_t threads)
    : _handler(scheduler), _threadPool(threadPool)
{
    assert(this->_handler && "Scheduler is required. ");
    if (this->_threadPool == nullptr)
    {
        assert(threads > 0 && "When not delcare thread pool, the threads parameter can't be zero. ");
        this->_ownWorkThread = true;
        this->_threadPool = Object::create<ThreadPool>(threads);
    }
}

Dispatcher::Dispatcher(StatefulWidget::State *state, Object::Ref<ThreadPool> threadPool, size_t threads)
{
    assert(state && "State is required. ");
    Object::Ref<BuildContext> context = state->element.lock();
    this->_handler = Scheduler::of(context);
    assert(this->_handler && "Can't find out scheduler in context. ");
    if (this->_threadPool == nullptr)
    {
        assert(threads > 0 && "When not delcare thread pool, the threads parameter can't be zero. ");
        this->_ownWorkThread = true;
        this->_threadPool = Object::create<ThreadPool>(threads);
    }
}

Dispatcher::~Dispatcher()
{
    assert((!this->_ownWorkThread || !this->_threadPool->isActive()) && "Detect memory leak from Dispatcher. Call [dispose]");
}

void Dispatcher::dispose()
{
    if (this->_ownWorkThread)
        this->_threadPool->dispose();
}
