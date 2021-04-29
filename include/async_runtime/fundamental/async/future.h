#pragma once

#include "../async.h"

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
    ref<ThreadPool> _callbackHandler;
    std::atomic_bool _completed;

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
    static ref<Future<void>> race(State<StatefulWidget> *state, ref<Set<ref<Future<>>>> set);
    static ref<Future<void>> wait(State<StatefulWidget> *state, ref<Set<ref<Future<>>>> set);

    static ref<Future<void>> value(ref<ThreadPool> callbackHandler);
    static ref<Future<void>> value(State<StatefulWidget> *state);

    static ref<Future<void>> delay(ref<ThreadPool> callbackHandler, Duration duration, Function<void()> onTimeout = nullptr);
    static ref<Future<void>> delay(State<StatefulWidget> *state, Duration duration, Function<void()> onTimeout = nullptr);

    Future(ref<ThreadPool> callbackHandler) : Future<std::nullptr_t>(callbackHandler), _callbackList(Object::create<List<Function<void()>>>()) {}
    Future(State<StatefulWidget> *state) : Future<std::nullptr_t>(state), _callbackList(Object::create<List<Function<void()>>>()) {}

    template <typename ReturnType = void>
    ref<Future<ReturnType>> than(Function<ReturnType()>);
    ref<Future<std::nullptr_t>> than(Function<void()>) override;

    virtual ref<Future<void>> timeout(Duration, Function<void()> onTimeout = nullptr);

protected:
    ref<List<Function<void()>>> _callbackList;
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

    Future(ref<ThreadPool> callbackHandler) : Future<std::nullptr_t>(callbackHandler), _data(), _callbackList(Object::create<List<Function<void(const T &)>>>()) {}
    Future(State<StatefulWidget> *state) : Future<std::nullptr_t>(state), _data(), _callbackList(Object::create<List<Function<void(const T &)>>>()) {}

    Future(ref<ThreadPool> callbackHandler, const T &data)
        : Future<std::nullptr_t>(callbackHandler), _data(data), _callbackList(Object::create<List<Function<void(const T &)>>>()) { this->_completed = true; }
    Future(State<StatefulWidget> *state, const T &data)
        : Future<std::nullptr_t>(state), _data(data), _callbackList(Object::create<List<Function<void(const T &)>>>()) { this->_completed = true; }

    template <typename ReturnType = void, typename std::enable_if<!std::is_void<ReturnType>::value>::type * = nullptr>
    ref<Future<ReturnType>> than(Function<ReturnType(const T &)>);
    template <typename ReturnType = void, typename std::enable_if<std::is_void<ReturnType>::value>::type * = nullptr>
    ref<Future<ReturnType>> than(Function<ReturnType(const T &)> fn);
    ref<Future<std::nullptr_t>> than(Function<void()>) override;

    virtual ref<Future<T>> timeout(Duration, Function<T()> onTimeout);

protected:
    T _data;
    ref<List<Function<void(const T &)>>> _callbackList;
};
