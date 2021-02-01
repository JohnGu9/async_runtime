#pragma once

#include "../async.h"

template <>
class Future<std::nullptr_t> : public Object, StateHelper
{
    template <typename R>
    friend class Completer;

    template <typename R>
    friend class Stream;

protected:
    Future(Object::Ref<ThreadPool> callbackHandler) : _callbackHandler(callbackHandler), _completed(false) {}
    Future(State<StatefulWidget> *state) : _callbackHandler(getHandlerfromState(state)), _completed(false) {}
    std::atomic_bool _completed;
    Object::Ref<ThreadPool> _callbackHandler;

public:
    static Object::Ref<Future<void>> race(Set<Object::Ref<Future<>>> &&set);
    static Object::Ref<Future<void>> wait(Set<Object::Ref<Future<>>> &&set);

    virtual Object::Ref<Future<void>> than(Function<void()>) = 0;
    virtual void sync() = 0;
    virtual void sync(Duration timeout) = 0;
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
    static Object::Ref<Future<void>> value(Object::Ref<ThreadPool> callbackHandler);
    static Object::Ref<Future<void>> value(State<StatefulWidget> *state);

    Future(Object::Ref<ThreadPool> callbackHandler) : Future<std::nullptr_t>(callbackHandler) {}
    Future(State<StatefulWidget> *state) : Future<std::nullptr_t>(state) {}

    template <typename ReturnType = void>
    Object::Ref<Future<ReturnType>> than(Function<ReturnType()>);
    Object::Ref<Future<void>> than(Function<void()>) override;

    virtual Object::Ref<Future<void>> timeout(Duration, Function<void()> onTimeout);
    void sync() override;
    void sync(Duration timeout) override;

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

    Future(Object::Ref<ThreadPool> callbackHandler) : Future<std::nullptr_t>(callbackHandler) {}
    Future(State<StatefulWidget> *state) : Future<std::nullptr_t>(state) {}

    Future(Object::Ref<ThreadPool> callbackHandler, const T &data)
        : _data(data), Future<std::nullptr_t>(callbackHandler) { this->_completed = true; }
    Future(State<StatefulWidget> *state, const T &data)
        : _data(data), Future<std::nullptr_t>(state) { this->_completed = true; }

    template <typename ReturnType = void, typename std::enable_if<!std::is_void<ReturnType>::value>::type * = nullptr>
    Object::Ref<Future<ReturnType>> than(Function<ReturnType(const T &)>);
    template <typename ReturnType = void, typename std::enable_if<std::is_void<ReturnType>::value>::type * = nullptr>
    Object::Ref<Future<ReturnType>> than(Function<ReturnType(const T &)> fn);
    Object::Ref<Future<void>> than(Function<void()>) override;

    virtual Object::Ref<Future<T>> timeout(Duration, Function<void()> onTimeout);
    void sync() override;
    void sync(Duration timeout) override;

protected:
    T _data;
    List<Function<void(const T &)>> _callbackList;
};