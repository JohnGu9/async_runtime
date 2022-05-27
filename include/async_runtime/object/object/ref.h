#pragma once
#include "ref_implement.h"

template <typename T>
class ref : public _async_runtime::RefImplement<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

public:
    ref(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : _async_runtime::RefImplement<T>(other) {}

protected:
    ref() {} // reserve for lateref

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : _async_runtime::RefImplement<T>(other) {}
};
