#pragma once

#include <future>
#include "dispatcher.h"

template <typename T = nullptr_t>
class Future;

template <>
class Future<nullptr_t> : public Dispatcher
{
protected:
    Future(State<StatefulWidget> *state) : Dispatcher(state) {}
    Future(Scheduler::Handler handler, Object::Ref<ThreadPool> threadPool = nullptr, size_t threads = 1)
        : Dispatcher(handler, threadPool, threads) {}
};

template <typename T>
class Future : public Future<nullptr_t>
{
    template <typename ReturnType>
    Object::Ref<Future<ReturnType>> than(Function<ReturnType(T)>);
    T sync();

protected:
    std::future<T> _future;
    Object::List<Function<ReturnType(T)>> _callbackList;
};
