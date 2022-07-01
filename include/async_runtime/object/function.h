#pragma once

#include "object.h"
#include <functional>
#include <type_traits>

template <>
class Fn<Object::Void> : public virtual Object
{
};

template <typename ReturnType, class... Args>
class Fn<ReturnType(Args...)> : public Fn<Object::Void>, protected std::function<ReturnType(Args...)>
{
    using super = std::function<ReturnType(Args...)>;

public:
    Fn(std::nullptr_t) = delete;

    using super::function;
    using super::operator();
    using Fn<Object::Void>::operator==;

    const super &toStdFunction() const { return *this; }
};

template <typename ReturnType, class... Args>
class ref<Fn<ReturnType(Args...)>> : public _async_runtime::RefImplement<Fn<ReturnType(Args...)>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Fn<ReturnType(Args...)>>;

public:
    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) noexcept : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    ref(ref<R> &&other) noexcept : super(std::move(other)) {}

    template <typename Lambda, typename std::enable_if<std::is_constructible<std::function<ReturnType(Args...)>, Lambda>::value>::type * = nullptr>
    ref(Lambda lambda) : super(Object::create<Fn<ReturnType(Args...)>>(std::forward<Lambda>(lambda))) {}

    ReturnType operator()(Args... args) const { return this->get()->operator()(std::forward<Args>(args)...); }

protected:
    ref() noexcept : super() {}
    using _async_runtime::RefImplement<Fn<ReturnType(Args...)>>::RefImplement;
};
