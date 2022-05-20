#pragma once

#include "../object.h"
#include <functional>
#include <memory>
#include <type_traits>
/**
 * @brief Function object in Async Runtime
 *
 *
 * @tparam T
 * the function format
 *
 * @example
 * Function<void()> fn = []() -> void {};
 *
 */
template <typename T = std::nullptr_t>
class Fn;

template <>
class Fn<std::nullptr_t> : public virtual Object
{
};

template <typename ReturnType, class... Args>
class Fn<ReturnType(Args...)> : public Fn<std::nullptr_t>, protected std::function<ReturnType(Args...)>
{
    static_assert(std::is_constructible<std::function<ReturnType(Args...)>, ReturnType(Args...)>::value, "Can't construct object in function");

public:
    Fn(std::nullptr_t) = delete;
    template <typename Lambda, typename std::enable_if<std::is_constructible<std::function<ReturnType(Args...)>, Lambda>::value>::type * = nullptr>
    Fn(Lambda lambda) : std::function<ReturnType(Args...)>(lambda) {}

    ReturnType operator()(Args... args) const { return std::function<ReturnType(Args...)>::operator()(std::forward<Args>(args)...); }
    const std::function<ReturnType(Args...)> &toStdFunction() const { return *this; }
};

template <typename ReturnType, class... Args>
class ref<Fn<ReturnType(Args...)>> : public _async_runtime::RefImplement<Fn<ReturnType(Args...)>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

    template <typename X, typename Y>
    friend bool operator==(const ref<X> &object0, ref<Y> object1);

public:
    ref(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : _async_runtime::RefImplement<Fn<ReturnType(Args...)>>(other) {}

    template <typename Lambda, typename std::enable_if<std::is_constructible<std::function<ReturnType(Args...)>, Lambda>::value>::type * = nullptr>
    ref(Lambda lambda) : _async_runtime::RefImplement<Fn<ReturnType(Args...)>>(std::make_shared<Fn<ReturnType(Args...)>>(lambda)) {}

    ReturnType operator()(Args... args) const { return (*this)->operator()(std::forward<Args>(args)...); }

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : _async_runtime::RefImplement<Fn<ReturnType(Args...)>>(other) {}
};

template <typename T>
using Function = ref<Fn<T>>;

template <typename ReturnType, class... Args>
class lateref<Fn<ReturnType(Args...)>> : public ref<Fn<ReturnType(Args...)>>
{
public:
    explicit lateref() {}

    lateref(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    lateref(const ref<R> &other) : ref<Fn<ReturnType(Args...)>>(other) {}

    template <typename Lambda, typename std::enable_if<std::is_constructible<std::function<ReturnType(Args...)>, Lambda>::value>::type * = nullptr>
    lateref(Lambda lambda) : ref<Fn<ReturnType(Args...)>>(std::make_shared<Fn<ReturnType(Args...)>>(lambda)) {}
};
