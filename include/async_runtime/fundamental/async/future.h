#pragma once
#include "../async.h"
#include <tuple>

template <typename T>
class FutureOr;

template <>
class Future<Object::Void> : public virtual Object, public EventLoopGetterMixin
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;

protected:
    static void _setCallback(Function<void()> callback) {}

    template <class T, class... _Args>
    static void _setCallback(Function<void()> callback, ref<Future<T>> future, ref<Future<_Args>>... __args)
    {
        future->then(callback);
        Future<>::_setCallback(callback, __args...);
    }

    Future(ref<EventLoop> loop) : _loop(loop), _completed(false) {}
    ref<EventLoop> _loop;
    bool _completed;

public:
    /**
     * @brief For [Future::wait] and [Future::race] return value wrapper
     *
     * Under [Future::wait], all [Package::getValue<Index>()] is safe to access.
     * But under [Future::race], it's not. You need to check every [Package::completed<Index>()] completed status before you access.
     *
     * @tparam T Future<T>
     */
    template <class... _Args>
    class Package;

    template <class T, class... _Args>
    static ref<Future<ref<Package<T, _Args...>>>> wait(ref<Future<T>> future, ref<Future<_Args>>... __args);

    template <class T, class... _Args>
    static ref<Future<ref<Package<T, _Args...>>>> race(ref<Future<T>> future, ref<Future<_Args>>... __args);

    ref<EventLoop> eventLoop() override { return _loop; }
    bool completed() const { return _completed; }
};

template <typename T>
class Future : public Future<Object::Void>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;
    static ref<List<Function<void(const T &)>>> dummy;

public:
    using DataType = T;
    static ref<Future<T>> async(Function<T()> fn);
    static ref<Future<T>> toEventLoop(Function<T()> fn, ref<EventLoop> eventLoop);

    static ref<Future<T>> delay(Duration timeout, Function<T()> fn, option<EventLoopGetterMixin> getter = nullptr);
    static ref<Future<T>> delay(Duration timeout, T value, option<EventLoopGetterMixin> getter = nullptr);

    static ref<Future<T>> value(const T &, option<EventLoopGetterMixin> getter = nullptr);
    static ref<Future<T>> value(T &&, option<EventLoopGetterMixin> getter = nullptr);

    Future(option<EventLoopGetterMixin> getter = nullptr)
        : Future<Object::Void>(EventLoopGetterMixin::ensureEventLoop(getter)), _data(),
          _callbackList(List<Function<void(const T &)>>::create()) {}

    template <typename ReturnType = Object::Void>
    ref<Future<ReturnType>> then(Function<FutureOr<ReturnType>(const T &)>);
    template <typename ReturnType = Object::Void>
    ref<Future<ReturnType>> then(Function<FutureOr<ReturnType>()>);

    virtual ref<Future<T>> then(Function<void(const T &)>);
    virtual ref<Future<T>> then(Function<void()>);

    virtual ref<Future<T>> timeout(Duration, Function<T()> onTimeout);

protected:
    virtual void complete(const T &);
    virtual void complete(T &&);
    T _data;
    ref<List<Function<void(const T &)>>> _callbackList;
};

template <class... _Args>
class Future<>::Package : public virtual Object, public std::tuple<ref<Future<_Args>>...>
{
    _ASYNC_RUNTIME_FRIEND_ASYNC_FAMILY;
    using super = std::tuple<ref<Future<_Args>>...>;

public:
    template <std::size_t Index>
    using FutureType = typename std::tuple_element<Index, super>::type;
    template <std::size_t Index>
    using DataType = typename std::tuple_element<Index, std::tuple<_Args...>>::type;

    Package(ref<Future<_Args>>... __args) : super(__args...) {}

    template <std::size_t Index>
    const FutureType<Index> &getFuture() const
    {
        return std::get<Index>(static_cast<const super &>(*this));
    }

    template <std::size_t Index>
    const DataType<Index> &getValue() const
    {
        return this->template getFuture<Index>()->_data;
    }

    template <std::size_t Index>
    bool completed() const
    {
        return this->template getFuture<Index>()->completed();
    }
};

template <class T, class... _Args>
ref<Future<ref<Future<>::Package<T, _Args...>>>> Future<>::wait(ref<Future<T>> future, ref<Future<_Args>>... __args)
{
    auto counter = new size_t(0);
    auto package = Object::create<Package<T, _Args...>>(future, __args...);
    auto completer = Object::create<Completer<ref<Package<T, _Args...>>>>();
    Function<void()> onCompleted = [counter, completer, package] //
    {                                                            //
        if (++(*counter) == (sizeof...(_Args) + 1))
        {
            completer->complete(package);
            delete counter;
        }
    };
    Future<>::_setCallback(onCompleted, future, __args...);
    return completer;
}

template <class T, class... _Args>
ref<Future<ref<Future<>::Package<T, _Args...>>>> Future<>::race(ref<Future<T>> future, ref<Future<_Args>>... __args)
{
    auto package = Object::create<Package<T, _Args...>>(future, __args...);
    auto completer = Object::create<Completer<ref<Package<T, _Args...>>>>();
    Function<void()> onCompleted = [completer, package] //
    {                                                   //
        if (!completer->completed())
        {
            completer->complete(package);
        }
    };
    Future<>::_setCallback(onCompleted, future, __args...);
    return completer;
}

template <typename T>
ref<List<Function<void(const T &)>>> Future<T>::dummy = List<Function<void(const T &)>>::create();

template <typename T>
ref<Future<T>> Future<T>::async(Function<T()> fn)
{
    auto loop = EventLoopGetterMixin::ensureEventLoop(nullptr);
    auto future = Object::create<Completer<T>>(loop);
    loop->callSoon([future, fn]
                   { future->complete(fn()); });
    return future;
}

template <typename T>
ref<Future<T>> Future<T>::toEventLoop(Function<T()> fn, ref<EventLoop> eventLoop)
{
    auto future = Object::create<Completer<T>>();
    eventLoop->callSoonThreadSafe(
        [fn, future] //
        {            //
            future->eventLoop()->callSoonThreadSafe(std::bind(
                [future](T &value)
                { future->complete(std::move(value)); },
                fn()));
        });
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

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4573)
#endif
template <typename T>
template <typename ReturnType>
ref<Future<ReturnType>> Future<T>::then(Function<FutureOr<ReturnType>(const T &)> fn)
{
    if (this->_completed)
    {
        FutureOr<ReturnType> result = fn(_data);
        auto &future = result._future;
        if_not_null(future) return future;
        else_end()
        {
            auto future = Object::create<Completer<ReturnType>>(self());
            future->complete(result._value);
            return future;
        }
        end_if();
    }
    else
    {
        ref<Completer<ReturnType>> future = Object::create<Completer<ReturnType>>(self());
        this->_callbackList->emplaceBack([future, fn](const T &value) { //
            FutureOr<ReturnType> result = fn(value);
            auto &resultFuture = result._future;
            if_not_null(resultFuture)
                resultFuture->then([future](const ReturnType &v) { //
                    future->complete(v);
                });
            else_end()
                future->complete(result._value);
            end_if();
        });
        return future;
    }
}
#ifdef _WIN32
#pragma warning(pop)
#endif

template <typename T>
template <typename ReturnType>
ref<Future<ReturnType>> Future<T>::then(Function<FutureOr<ReturnType>()> fn)
{
    return this->then<ReturnType>([fn](const T &)
                                  { return fn(); });
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
        this->_callbackList->emplaceBack(fn);
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
        this->_callbackList->emplaceBack([fn](const T &value)
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
    template <typename>
    friend class Future;

public:
    FutureOr(T directlyReturn) : _value(directlyReturn) {}
    FutureOr(ref<Future<T>> asyncReturn) : _future(asyncReturn) {}

protected:
    T _value;
    option<Future<T>> _future;
};

template <typename T>
class AsyncSnapshot<T>::_FromFuture : public AsyncSnapshot<T>
{
    using super = AsyncSnapshot<T>;

protected:
    ref<Future<T>> _future;

public:
    _FromFuture(ref<Future<T>> future)
        : super(future->completed() ? ConnectionState::done : ConnectionState::active), _future(future) {}

    bool hasData() noexcept override { return _future->completed(); }
    const T &data() override
    {
        if (this->hasData())
            return _future->_data;
        else
            throw std::runtime_error("AsyncSnapshot has no data");
    };
};

template <typename T>
ref<AsyncSnapshot<T>> AsyncSnapshot<T>::fromFuture(ref<Future<T>> future)
{
    return Object::create<AsyncSnapshot<T>::_FromFuture>(future);
}
