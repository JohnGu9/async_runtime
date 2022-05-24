#pragma once
#include "ref_implement.h"

template <typename T>
class ref : public _async_runtime::RefImplement<T>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;

    template <typename X, typename Y>
    friend bool operator==(const ref<X> &object0, ref<Y> object1);
    template <typename X, typename Y>
    friend bool operator!=(const ref<X> &object0, ref<Y> object1);

    template <typename X, typename Y>
    friend bool operator==(const option<X> &object0, ref<Y> object1);
    template <typename X, typename Y>
    friend bool operator!=(const option<X> &object0, ref<Y> object1);

    template <typename X, typename Y>
    friend bool operator==(const ref<X> &object0, option<Y> object1);
    template <typename X, typename Y>
    friend bool operator!=(const ref<X> &object0, option<Y> object1);

public:
    ref(std::nullptr_t) = delete;

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : _async_runtime::RefImplement<T>(other) {}

protected:
    ref() {}

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : _async_runtime::RefImplement<T>(other) {}
};
