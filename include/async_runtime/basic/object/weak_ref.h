#pragma once
#include "declare.h"

template <typename T>
class weakref : public std::weak_ptr<T>, public _async_runtime::ToRefMixin<T>
{
public:
    weakref() {}
    weakref(std::nullptr_t) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    weakref(const option<R> &other) : std::weak_ptr<T>(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    weakref(const ref<R> &other) : std::weak_ptr<T>(other) {}

    std::weak_ptr<T> lock() const = delete;
    ref<T> assertNotNull() const override;
    ref<T> isNotNullElse(Function<ref<T>()> fn) const override;
    bool isNotNull(ref<T> &object) const override;
    option<T> toOption() const;
};
