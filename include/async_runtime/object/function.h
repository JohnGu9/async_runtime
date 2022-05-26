#pragma once

#include "object.h"
#include <functional>
#include <memory>
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
};

template <typename ReturnType, class... Args>
class ref<Fn<ReturnType(Args...)>> : public _async_runtime::RefImplement<Fn<ReturnType(Args...)>>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    ref(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : _async_runtime::RefImplement<Fn<ReturnType(Args...)>>(other) {}

    template <typename Lambda, typename std::enable_if<std::is_constructible<std::function<ReturnType(Args...)>, Lambda>::value>::type * = nullptr>
    ref(Lambda lambda) : _async_runtime::RefImplement<Fn<ReturnType(Args...)>>(std::make_shared<Fn<ReturnType(Args...)>>(lambda)) {}

    ReturnType operator()(Args... args) const { return (*this)->operator()(std::forward<Args>(args)...); }

    template <typename R>
    bool operator==(const ref<R> &object1) const;
    template <typename R>
    bool operator!=(const ref<R> &other) const { return !this->operator==(other); }
    template <typename R>
    bool operator==(ref<R> &&object1) const;
    template <typename R>
    bool operator!=(ref<R> &&other) const { return !this->operator==(std::move(other)); }

    template <typename R>
    bool operator==(const option<R> &object1) const;
    template <typename R>
    bool operator!=(const option<R> &other) const { return !this->operator==(other); }
    template <typename R>
    bool operator==(option<R> &&object1) const;
    template <typename R>
    bool operator!=(option<R> &&other) const { return !this->operator==(std::move(other)); }

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<Fn<ReturnType(Args...)>, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : _async_runtime::RefImplement<Fn<ReturnType(Args...)>>(other) {}
};

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
