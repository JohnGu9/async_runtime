#pragma once
#include "ref_implement.h"

template <typename T>
class ref : public _async_runtime::RefImplement<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<T>;

public:
    ref(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(std::move(other)) {}

protected:
    ref() {} // reserve for lateref

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(std::shared_ptr<R> &&other) : super(std::move(other)) {}
};
