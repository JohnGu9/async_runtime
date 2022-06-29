#pragma once

#include "object.h"
#include <functional>
#include <type_traits>

template <>
class Fn<std::nullptr_t> : public virtual Object
{
};

template <typename ReturnType, class... Args>
class Fn<ReturnType(Args...)> : public Fn<std::nullptr_t>, protected std::function<ReturnType(Args...)>
{
    using super = std::function<ReturnType(Args...)>;

public:
    Fn(std::nullptr_t) = delete;

    using super::function;
    using super::operator();

    const super &toStdFunction() const { return *this; }

    /* Apple clang compile bug patch. And this is useless code can be removed if apple fix the bug */
    bool operator==(ref<Object> other) override { return this->Object::operator==(other); };
};

template <typename ReturnType, class... Args>
class ref<Fn<ReturnType(Args...)>> : public _async_runtime::RefImplement<Fn<ReturnType(Args...)>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Fn<ReturnType(Args...)>>;

public:
    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(std::move(other)) {}

    template <typename Lambda, typename std::enable_if<std::is_constructible<std::function<ReturnType(Args...)>, Lambda>::value>::type * = nullptr>
    ref(Lambda lambda) : super(Object::create<Fn<ReturnType(Args...)>>(std::forward<Lambda>(lambda))) {}

    ReturnType operator()(Args... args) const { return (*this)->operator()(std::forward<Args>(args)...); }

protected:
    ref() {}
    using _async_runtime::RefImplement<Fn<ReturnType(Args...)>>::RefImplement;
};
