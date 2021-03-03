#pragma once

#include "../async.h"

template <typename T>
class FutureOr
{
public:
    FutureOr(T &&value) : _value(value), _future(nullptr) {}
    FutureOr(ref<Future<T>> future) : _value(nullptr), _future(future) { assert(_future != nullptr); }

    ref<Future<T>> toFuture() { return _future; }
    T &toValue() { return _value; }

protected:
    T &_value;
    ref<Future<T>> _future;
};

template <>
class Future<std::nullptr_t> : public virtual Object, protected StateHelper
{
    template <typename R>
    friend class Completer;

    template <typename R>
    friend class Stream;

    template <typename R>
    friend class AsyncSnapshot;

protected:
    Future(ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _completed(false) {}
    Future(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)), _completed(false) {}
    std::atomic_bool _completed;
    ref<ThreadPool> _callbackHandler;

public:
    virtual ref<Future<std::nullptr_t>> than(Function<void()>) = 0;

    // not recommend to use these two functions
    // async function should always be async
    // try to sync may cause deadlock
    virtual void sync();
    virtual void sync(Duration timeout);
};

template <>
class Future<void> : public Future<std::nullptr_t>
{
    template <typename R>
    friend class Completer;

    template <typename R>
    friend class Stream;

    template <typename R>
    friend class AsyncSnapshot;

    template <typename R>
    friend ref<Future<R>> async(ref<ThreadPool> callbackHandler, Function<R()> fn);

    template <typename R>
    friend ref<Future<R>> async(State<StatefulWidget> *state, Function<R()> fn);

public:
    static ref<Future<void>> race(State<StatefulWidget> *state, Set<ref<Future<>>> &&set);
    static ref<Future<void>> wait(State<StatefulWidget> *state, Set<ref<Future<>>> &&set);

    static ref<Future<void>> value(ref<ThreadPool> callbackHandler);
    static ref<Future<void>> value(State<StatefulWidget> *state);

    static ref<Future<void>> delay(ref<ThreadPool> callbackHandler, Duration duration, Function<void()> onTimeout = nullptr);
    static ref<Future<void>> delay(State<StatefulWidget> *state, Duration duration, Function<void()> onTimeout = nullptr);

    Future(ref<ThreadPool> callbackHandler) : Future<std::nullptr_t>(callbackHandler), _callbackList({}) {}
    Future(State<StatefulWidget> *state) : Future<std::nullptr_t>(state), _callbackList({}) {}

    template <typename ReturnType = void>
    ref<Future<ReturnType>> than(Function<ReturnType()>);
    ref<Future<std::nullptr_t>> than(Function<void()>) override;

    virtual ref<Future<void>> timeout(Duration, Function<void()> onTimeout = nullptr);

protected:
    List<Function<void()>> _callbackList;
};

template <typename T>
class Future : public Future<std::nullptr_t>
{
    template <typename R>
    friend class Completer;

    template <typename R>
    friend class Stream;

    template <typename R>
    friend class AsyncSnapshot;

    template <typename R>
    friend ref<Future<R>> async(ref<ThreadPool> callbackHandler, Function<R()> fn);

    template <typename R>
    friend ref<Future<R>> async(State<StatefulWidget> *state, Function<R()> fn);

public:
    static ref<Future<T>> value(ref<ThreadPool> callbackHandler, const T &);
    static ref<Future<T>> value(State<StatefulWidget> *state, const T &);
    static ref<Future<T>> value(ref<ThreadPool> callbackHandler, T &&);
    static ref<Future<T>> value(State<StatefulWidget> *state, T &&);

    Future(ref<ThreadPool> callbackHandler) : Future<std::nullptr_t>(callbackHandler), _callbackList({}) {}
    Future(State<StatefulWidget> *state) : Future<std::nullptr_t>(state), _callbackList({}) {}

    Future(ref<ThreadPool> callbackHandler, const T &data)
        : _data(data), _callbackList({}), Future<std::nullptr_t>(callbackHandler) { this->_completed = true; }
    Future(State<StatefulWidget> *state, const T &data)
        : _data(data), _callbackList({}), Future<std::nullptr_t>(state) { this->_completed = true; }

    template <typename ReturnType = void, typename std::enable_if<!std::is_void<ReturnType>::value>::type * = nullptr>
    ref<Future<ReturnType>> than(Function<ReturnType(const T &)>);
    template <typename ReturnType = void, typename std::enable_if<std::is_void<ReturnType>::value>::type * = nullptr>
    ref<Future<ReturnType>> than(Function<ReturnType(const T &)> fn);
    ref<Future<std::nullptr_t>> than(Function<void()>) override;

    virtual ref<Future<T>> timeout(Duration, Function<T()> onTimeout);

protected:
    T _data;
    List<Function<void(const T &)>> _callbackList;
};
