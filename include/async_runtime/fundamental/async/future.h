#pragma once

#include "../async.h"

template <typename T>
class FutureOr;

template <>
class Future<std::nullptr_t> : public Object, protected StateHelper
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

protected:
    Future(ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _completed(false) {}
    ref<ThreadPool> _callbackHandler;
    std::atomic_bool _completed;

public:
    virtual ref<Future<std::nullptr_t>> than(Function<void()>) = 0;

    // not recommend to use [Future::sync]
    // async function should always be async
    // try to sync may cause deadlock
    virtual void sync();
    virtual void sync(Duration timeout);
};

template <>
class Future<void> : public Future<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

    template <typename R>
    friend ref<Future<R>> async(ref<ThreadPool> callbackHandler, Function<R()> fn);

    template <typename R>
    friend ref<Future<R>> async(ref<State<StatefulWidget>> state, Function<R()> fn);

public:
    static ref<Future<void>> race(ref<State<StatefulWidget>> state, ref<Set<ref<Future<>>>> set);
    static ref<Future<void>> wait(ref<State<StatefulWidget>> state, ref<Set<ref<Future<>>>> set);

    static ref<Future<void>> value(ref<ThreadPool> callbackHandler);
    static ref<Future<void>> value(ref<State<StatefulWidget>> state);

    static ref<Future<void>> delay(ref<ThreadPool> callbackHandler, Duration duration, option<Fn<void()>> onTimeout = nullptr);
    static ref<Future<void>> delay(ref<State<StatefulWidget>> state, Duration duration, option<Fn<void()>> onTimeout = nullptr);

    Future(ref<ThreadPool> callbackHandler) : Future<std::nullptr_t>(callbackHandler), _callbackList(Object::create<List<Function<void()>>>()) {}

    template <typename ReturnType>
    ref<Future<ReturnType>> than(Function<FutureOr<ReturnType>()>);
    ref<Future<std::nullptr_t>> than(Function<void()>) override;

    virtual ref<Future<void>> timeout(Duration, option<Fn<void()>> onTimeout = nullptr);

protected:
    ref<List<Function<void()>>> _callbackList;
};

template <typename T>
class Future : public Future<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

    template <typename R>
    friend ref<Future<R>> async(ref<ThreadPool> callbackHandler, Function<R()> fn);

    template <typename R>
    friend ref<Future<R>> async(ref<State<StatefulWidget>> state, Function<R()> fn);

public:
    static ref<Future<T>> value(ref<ThreadPool> callbackHandler, const T &);
    static ref<Future<T>> value(ref<State<StatefulWidget>> state, const T &);
    static ref<Future<T>> value(ref<ThreadPool> callbackHandler, T &&);
    static ref<Future<T>> value(ref<State<StatefulWidget>> state, T &&);

    Future(ref<ThreadPool> callbackHandler)
        : Future<std::nullptr_t>(callbackHandler), _data(),
          _callbackList(Object::create<List<Function<void(const T &)>>>()) {}

    Future(ref<ThreadPool> callbackHandler, const T &data)
        : Future<std::nullptr_t>(callbackHandler), _data(data),
          _callbackList(Object::create<List<Function<void(const T &)>>>()) { this->_completed = true; }

    template <typename ReturnType, typename std::enable_if<!std::is_void<ReturnType>::value>::type * = nullptr>
    ref<Future<ReturnType>> than(Function<FutureOr<ReturnType>(const T &)>);
    template <typename ReturnType, typename std::enable_if<std::is_void<ReturnType>::value>::type * = nullptr>
    ref<Future<ReturnType>> than(Function<FutureOr<ReturnType>(const T &)>);
    ref<Future<std::nullptr_t>> than(Function<void()>) override;

    virtual ref<Future<T>> timeout(Duration, Function<T()> onTimeout);

protected:
    T _data;
    ref<List<Function<void(const T &)>>> _callbackList;
};

template <typename T>
class FutureOr
{
    template <typename R>
    friend class Future;

public:
    FutureOr(T directlyReturn) : _value(directlyReturn) {}
    FutureOr(ref<Future<T>> asyncReturn) : _future(asyncReturn) {}

protected:
    T _value;
    option<Future<T>> _future;
};

template <>
class FutureOr<void>
{
    template <typename R>
    friend class Future;

public:
    FutureOr() {}
    FutureOr(ref<Future<void>> asyncReturn) : _future(asyncReturn) {}

protected:
    option<Future<void>> _future;
};