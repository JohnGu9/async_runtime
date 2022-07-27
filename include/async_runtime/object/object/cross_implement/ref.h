#pragma once
#include "../ref_implement.h"

template <typename T>
template <typename OTHER, typename std::enable_if<_async_runtime::RefFilter<OTHER>::value>::type *>
bool _async_runtime::RefImplement<T>::operator==(const OTHER &other) const
{
    return this->get()->operator==(other);
}

template <typename T>
bool _async_runtime::RefImplement<T>::operator==(const std::nullptr_t &) const
{
    return this->get()->operator==(option<Object>());
}

template <typename T>
template <typename OTHER, typename std::enable_if<_async_runtime::RefFilter<OTHER>::value>::type *>
bool _async_runtime::RefImplement<T>::operator<(const OTHER &other) const
{
    return this->get()->operator<(other);
}

template <typename T>
bool _async_runtime::RefImplement<T>::operator<(const std::nullptr_t &) const
{
    return this->get()->operator<(option<Object>());
}

template <typename T>
template <typename OTHER, typename std::enable_if<_async_runtime::RefFilter<OTHER>::value>::type *>
bool _async_runtime::RefImplement<T>::operator>(const OTHER &other) const
{
    return this->get()->operator>(other);
}

template <typename T>
bool _async_runtime::RefImplement<T>::operator>(const std::nullptr_t &) const
{
    return this->get()->operator>(option<Object>());
}
