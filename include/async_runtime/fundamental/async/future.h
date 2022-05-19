#pragma once

#include "../async.h"

template <typename T>
class FutureOr;

template <>
class Future<std::nullptr_t> : public virtual Object, public EventLoopGetterMixin
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

protected:
    Future(ref<EventLoop> loop) : _loop(loop), _completed(false) {}
    ref<EventLoop> _loop;
    bool _completed;

public:
    /**
     * @brief For [Future::wait] and [Future::race] return value wrapper
     *
     * Under [Future::wait], all [Package::value] is safe to access.
     * But under [Future::race], it's not. You need to check every [Package::future] completed status before you access.
     *
     * @tparam T Future<T>
     */
    template <class T, class... _Args>
    class Package;

    template <class T, class... _Args>
    static ref<Future<ref<Package<T, _Args...>>>> wait(ref<Future<T>> future, ref<Future<_Args>>... __args);

    template <class T, class... _Args>
    static ref<Future<ref<Package<T, _Args...>>>> race(ref<Future<T>> future, ref<Future<_Args>>... __args);

    ref<EventLoop> eventLoop() override { return _loop; }
    bool completed() const { return _completed; }
};

template <typename T>
class Future : public Future<std::nullptr_t>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;
    static ref<List<Function<void(const T &)>>> dummy;

public:
    static ref<Future<T>> async(Function<T()> fn, option<EventLoopGetterMixin> getter = nullptr);
    static ref<Future<T>> delay(Duration timeout, Function<T()> fn, option<EventLoopGetterMixin> getter = nullptr);
    static ref<Future<T>> delay(Duration timeout, T value, option<EventLoopGetterMixin> getter = nullptr);

    static ref<Future<T>> value(const T &, option<EventLoopGetterMixin> getter = nullptr);
    static ref<Future<T>> value(T &&, option<EventLoopGetterMixin> getter = nullptr);

    Future(option<EventLoopGetterMixin> getter = nullptr)
        : Future<std::nullptr_t>(EventLoopGetterMixin::ensureEventLoop(getter)), _data(),
          _callbackList(Object::create<List<Function<void(const T &)>>>()) {}

    template <typename ReturnType = std::nullptr_t>
    ref<Future<ReturnType>> then(Function<FutureOr<ReturnType>(const T &)>);
    ref<Future<T>> then(Function<void(const T &)>);
    ref<Future<T>> then(Function<void()>);

    virtual ref<Future<T>> timeout(Duration, Function<T()> onTimeout);

protected:
    virtual void complete(const T &);
    virtual void complete(T &&);
    T _data;
    ref<List<Function<void(const T &)>>> _callbackList;
};

template <class T>
class Future<std::nullptr_t>::Package<T> : public virtual Object
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

public:
    Package(ref<Future<T>> future) : future(future) {}
    ref<Future<T>> future;
    const T &value = future->_data;

protected:
    void _setCallback(Function<void()> callback)
    {
        future->then(callback);
    }
};

template <class T, class... _Args>
class Future<std::nullptr_t>::Package : public virtual Object
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

public:
    Package(ref<Future<T>> future, ref<Future<_Args>>... __args)
        : future(future),
          next(Object::create<Package<_Args...>>(__args...)) {}

    ref<Future<T>> future;
    const T &value = future->_data;
    ref<Package<_Args...>> next;

protected:
    void _setCallback(Function<void()> callback)
    {
        future->then(callback);
        next->_setCallback(callback);
    }
};

template <class T, class... _Args>
ref<Future<ref<Future<>::Package<T, _Args...>>>> Future<>::wait(ref<Future<T>> future, ref<Future<_Args>>... __args)
{
    auto counter = new size_t(0);
    auto package = Object::create<Package<T, _Args...>>(future, __args...);
    auto completer = Object::create<Completer<ref<Package<T, _Args...>>>>();
    Function<void()> onCompleted = [counter, completer, package]() //
    {                                                              //
        (*counter)++;
        if (*counter == (sizeof...(_Args) + 1))
        {
            completer->complete(package);
            delete counter;
        }
    };
    package->_setCallback(onCompleted);
    return completer;
}

template <class T, class... _Args>
ref<Future<ref<Future<>::Package<T, _Args...>>>> Future<>::race(ref<Future<T>> future, ref<Future<_Args>>... __args)
{
    auto package = Object::create<Package<T, _Args...>>(future, __args...);
    auto completer = Object::create<Completer<ref<Package<T, _Args...>>>>();
    Function<void()> onCompleted = [completer, package]() //
    {                                                     //
        if (!completer->completed())
        {
            completer->complete(package);
        }
    };
    package->_setCallback(onCompleted);
    return completer;
}

template <typename T>
ref<List<Function<void(const T &)>>> Future<T>::dummy = Object::create<List<Function<void(const T &)>>>();

template <typename T>
ref<Future<T>> Future<T>::async(Function<T()> fn, option<EventLoopGetterMixin> getter)
{
    auto loop = EventLoopGetterMixin::ensureEventLoop(getter);
    auto future = Object::create<Completer<T>>(getter);
    loop->callSoon([future, fn]
                   { future->complete(fn()); });
    return future;
}

template <typename T>
void Future<T>::complete(const T &data)
{
    if (_completed)
        throw std::logic_error("Future can not be completed twice");
    _completed = true;
    _data = data;
    auto callbackList = _callbackList;
    _callbackList = dummy;
    for (const auto &element : callbackList)
        element(this->_data);
}

template <typename T>
void Future<T>::complete(T &&data)
{
    if (_completed)
        throw std::logic_error("Future can not be completed twice");
    _completed = true;
    _data = std::move(data);
    auto callbackList = _callbackList;
    _callbackList = dummy;
    for (const auto &element : callbackList)
        element(this->_data);
}

template <typename T>
ref<Future<T>> Future<T>::value(const T &value, option<EventLoopGetterMixin> getter)
{
    auto future = Object::create<Future<T>>(getter);
    future->complete(value);
    return future;
}

template <typename T>
ref<Future<T>> Future<T>::value(T &&value, option<EventLoopGetterMixin> getter)
{
    auto future = Object::create<Future<T>>(getter);
    future->complete(std::move(value));
    return future;
}

template <typename T>
template <typename ReturnType>
ref<Future<ReturnType>> Future<T>::then(Function<FutureOr<ReturnType>(const T &)> fn)
{
    if (this->_completed)
    {
        FutureOr<ReturnType> result = fn(_data);
        lateref<Future<ReturnType>> resultFuture;
        if (result._future.isNotNull(resultFuture))
        {
            return resultFuture;
        }
        ref<Completer<ReturnType>> future = Object::create<Completer<ReturnType>>(self());
        future->complete(result._value);
        return future;
    }
    else
    {
        ref<Completer<ReturnType>> future = Object::create<Completer<ReturnType>>(self());
        this->_callbackList
            ->emplace_back([future, fn](const T &value) //
                           {                            //
                               FutureOr<ReturnType> result = fn(value);
                               lateref<Future<ReturnType>> resultFuture;
                               if (result._future.isNotNull(resultFuture))
                               {
                                   resultFuture->template then<int>([future](const ReturnType &value) //
                                                                    {                                 //
                                                                        future->complete(value);
                                                                        return 0;
                                                                    });
                               }
                               else
                               {
                                   future->complete(result._value);
                               }
                           });
        return future;
    }
}

template <typename T>
ref<Future<T>> Future<T>::then(Function<void(const T &)> fn)
{
    if (this->_completed)
    {
        fn(_data);
    }
    else
    {
        this->_callbackList
            ->emplace_back(fn);
    }
    return self();
}

template <typename T>
ref<Future<T>> Future<T>::then(Function<void()> fn)
{
    if (this->_completed)
    {
        fn();
    }
    else
    {
        this->_callbackList
            ->emplace_back([fn](const T &value)
                           { fn(); });
    }
    return self();
}

template <typename T>
class Completer : public Future<T>
{
    using super = Future<T>;

public:
    Completer(option<EventLoopGetterMixin> getter = nullptr) : super(getter) {}
    void complete(const T &v) override { super::complete(v); }
    void complete(T &&v) override { super::complete(std::move(v)); }
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
