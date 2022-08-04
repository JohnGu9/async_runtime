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

    template <typename ReturnType, typename... Args>
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
    using super::super;

    template <typename Lambda, typename std::enable_if<std::is_constructible<Fn<ReturnType(Args...)>, Lambda>::value>::type * = nullptr>
    ref(Lambda lambda) : super(Object::create<Fn<ReturnType(Args...)>>(std::move(lambda))) {}

    ReturnType operator()(Args... args) const { return this->get()->operator()(std::forward<Args>(args)...); }

protected:
    ref() noexcept = default;
};

template <typename ReturnType, typename... Args>
class Fn<Object::Void>::BindFn : public Fn<ReturnType(Args...)>
{
    using super = Fn<ReturnType(Args...)>;

public:
    lateref<Object> object;
    using super::super;

    template <typename Lambda>
    BindFn(Lambda lambda, Object *object) : super(std::move(lambda)), object(::Object::cast<>(object)) {}
};

template <typename T, typename ReturnType, typename... Args>
Function<ReturnType(Args...)> Fn<Object::Void>::bind(ReturnType (T::*fn)(Args...), T *pointer)
{
    return Object::create<Fn<Object::Void>::BindFn<ReturnType, Args...>>(std::bind(fn, pointer), pointer);
}

/**
 * @brief bind a class member function
 * @warning bind function does not work with inheritance-override
 *
 * @example bind(memberFunction)
 *
 */
#define BIND_FUNCTION(__function__) ::Fn<Object::Void>::bind<>(&std::remove_pointer<decltype(this)>::type::__function__, this)
