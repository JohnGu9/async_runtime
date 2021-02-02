#pragma once

#include "../async.h"

template <>
class Future<std::nullptr_t> : public Object, protected StateHelper
{
    template <typename R>
    friend class Completer;

    template <typename R>
    friend class Stream;

protected:
    static bool every(const Set<Object::Ref<Future<>>> &set, Function<bool(Object::Ref<Future<>>)>);
    static bool any(const Set<Object::Ref<Future<>>> &set, Function<bool(Object::Ref<Future<>>)>);
    Future(Object::Ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _completed(false) {}
    Future(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)), _completed(false) {}
    std::atomic_bool _completed;
    Object::Ref<ThreadPool> _callbackHandler;

public:
    virtual Object::Ref<Future<std::nullptr_t>> than(Function<void()>) = 0;

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
    friend Object::Ref<Future<R>> async(Object::Ref<ThreadPool> callbackHandler, Function<R()> fn);

    template <typename R>
    friend Object::Ref<Future<R>> async(State<StatefulWidget> *state, Function<R()> fn);

public:
    static Object::Ref<Future<void>> race(State<StatefulWidget> *state, Set<Object::Ref<Future<>>> &&set);
    static Object::Ref<Future<void>> wait(State<StatefulWidget> *state, Set<Object::Ref<Future<>>> &&set);

    static Object::Ref<Future<void>> value(Object::Ref<ThreadPool> callbackHandler);
    static Object::Ref<Future<void>> value(State<StatefulWidget> *state);

    Future(Object::Ref<ThreadPool> callbackHandler) : Future<std::nullptr_t>(callbackHandler), _callbackList({}) {}
    Future(State<StatefulWidget> *state) : Future<std::nullptr_t>(state), _callbackList({}) {}

    template <typename ReturnType = void>
    Object::Ref<Future<ReturnType>> than(Function<ReturnType()>);
    Object::Ref<Future<std::nullptr_t>> than(Function<void()>) override;

    virtual Object::Ref<Future<void>> timeout(Duration, Function<void()> onTimeout);

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
    friend Object::Ref<Future<R>> async(Object::Ref<ThreadPool> callbackHandler, Function<R()> fn);

    template <typename R>
    friend Object::Ref<Future<R>> async(State<StatefulWidget> *state, Function<R()> fn);

public:
    static Object::Ref<Future<T>> value(Object::Ref<ThreadPool> callbackHandler, const T &);
    static Object::Ref<Future<T>> value(State<StatefulWidget> *state, const T &);
    static Object::Ref<Future<T>> value(Object::Ref<ThreadPool> callbackHandler, T &&);
    static Object::Ref<Future<T>> value(State<StatefulWidget> *state, T &&);

    Future(Object::Ref<ThreadPool> callbackHandler) : Future<std::nullptr_t>(callbackHandler), _callbackList({}) {}
    Future(State<StatefulWidget> *state) : Future<std::nullptr_t>(state), _callbackList({}) {}

    Future(Object::Ref<ThreadPool> callbackHandler, const T &data)
        : _data(data), _callbackList({}), Future<std::nullptr_t>(callbackHandler) { this->_completed = true; }
    Future(State<StatefulWidget> *state, const T &data)
        : _data(data), _callbackList({}), Future<std::nullptr_t>(state) { this->_completed = true; }

    template <typename ReturnType = void, typename std::enable_if<!std::is_void<ReturnType>::value>::type * = nullptr>
    Object::Ref<Future<ReturnType>> than(Function<ReturnType(const T &)>);
    template <typename ReturnType = void, typename std::enable_if<std::is_void<ReturnType>::value>::type * = nullptr>
    Object::Ref<Future<ReturnType>> than(Function<ReturnType(const T &)> fn);
    Object::Ref<Future<std::nullptr_t>> than(Function<void()>) override;

    virtual Object::Ref<Future<T>> timeout(Duration, Function<void()> onTimeout);

protected:
    T _data;
    List<Function<void(const T &)>> _callbackList;
};