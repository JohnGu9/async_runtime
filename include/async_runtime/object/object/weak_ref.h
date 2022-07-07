#pragma once
#include "declare.h"

template <typename T>
class weakref : protected std::weak_ptr<T>, public _async_runtime::ToRefMixin<T>
{
    using super = std::weak_ptr<T>;

public:
    weakref() {}
    weakref(std::nullptr_t) {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    weakref(const option<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    weakref(option<R> &&other) : super(std::move(other)) {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    weakref(const ref<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    weakref(ref<R> &&other) : super(std::move(other)) {}

    _async_runtime::Else ifNotNull(Function<void(ref<T>)> fn) const noexcept final;
    ref<T> ifNotNullElse(Function<ref<T>()> fn) const noexcept final;
    ref<T> assertNotNull() const noexcept(false) final;
    option<T> toOption() const noexcept;
};
