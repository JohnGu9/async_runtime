#pragma once

#include "object.h"
#include <functional>
#include <type_traits>

template <>
class Fn<Object::Void> : public virtual Object
{
public:
    template <typename T, typename ReturnType, typename... Args>
    static Function<ReturnType(Args...)> bind(ReturnType (T::*fn)(Args...), T *);

    template <typename T, typename ReturnType, typename... Args>
    class BindFn;
};

template <typename ReturnType, typename... Args>
class Fn<ReturnType(Args...)> : public Fn<Object::Void>, protected std::function<ReturnType(Args...)>
{
    using super = std::function<ReturnType(Args...)>;

public:
    Fn(std::nullptr_t) = delete;

    using super::super;
    using super::operator();
    using Fn<Object::Void>::operator==;

    const super &toStdFunction() const { return *this; }
};

template <typename ReturnType, typename... Args>
class ref<Fn<ReturnType(Args...)>> : public _async_runtime::RefImplement<Fn<ReturnType(Args...)>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Fn<ReturnType(Args...)>>;

public:
    template <typename T>
    static Function<ReturnType(Args...)> bind(ReturnType (T::*fn)(Args...), T *);

    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) noexcept : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    ref(ref<R> &&other) noexcept : super(std::move(other)) {}

    template <typename Lambda, typename std::enable_if<std::is_constructible<Fn<ReturnType(Args...)>, Lambda>::value>::type * = nullptr>
    ref(Lambda lambda) : super(Object::create<Fn<ReturnType(Args...)>>(std::move(lambda))) {}

    ReturnType operator()(Args... args) const { return this->get()->operator()(std::forward<Args>(args)...); }

protected:
    ref() noexcept : super() {}
    using super::super;
};

template <typename T, typename ReturnType, typename... Args>
class Fn<Object::Void>::BindFn : public Fn<ReturnType(Args...)>
{
    using super = Fn<ReturnType(Args...)>;

public:
    lateref<T> object;
    using super::super;
};

template <typename T, typename ReturnType, typename... Args>
Function<ReturnType(Args...)> Fn<Object::Void>::bind(ReturnType (T::*fn)(Args...), T *pointer)
{
    auto res = Object::create<Fn<Object::Void>::BindFn<T, ReturnType, Args...>>(std::bind(fn, pointer));
    res->object = Object::cast<>(pointer);
    return res;
}

template <typename ReturnType, typename... Args>
template <typename T>
Function<ReturnType(Args...)> Function<ReturnType(Args...)>::bind(ReturnType (T::*fn)(Args...), T *pointer)
{
    return Fn<Object::Void>::bind<>(fn, pointer);
}

#define BIND_FUNCTION(__function__) Fn<Object::Void>::bind<>(&std::remove_pointer<decltype(this)>::type::__function__, this)
