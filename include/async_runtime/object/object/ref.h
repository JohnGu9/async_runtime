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
    ref() {} // reserve for lateref

    template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type * = nullptr>
    ref(const std::shared_ptr<R> &other) : _async_runtime::RefImplement<T>(other) {}
};
