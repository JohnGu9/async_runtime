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
    static_assert(std::is_constructible<super, ReturnType(Args...)>::value, "Can't construct object in function");

public:
    Fn(std::nullptr_t) = delete;
    template <typename Lambda, typename std::enable_if<std::is_constructible<std::function<ReturnType(Args...)>, Lambda>::value>::type * = nullptr>
    Fn(Lambda lambda) : super(lambda) {}

    ReturnType operator()(Args... args) const { return super::operator()(std::forward<Args>(args)...); }
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
    ref(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}

    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(other) {}

    template <typename Lambda, typename std::enable_if<std::is_constructible<std::function<ReturnType(Args...)>, Lambda>::value>::type * = nullptr>
    ref(Lambda lambda) : super(std::make_shared<Fn<ReturnType(Args...)>>(lambda)) {}

    ReturnType operator()(Args... args) const { return (*this)->operator()(std::forward<Args>(args)...); }

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : super(other) {}
};
