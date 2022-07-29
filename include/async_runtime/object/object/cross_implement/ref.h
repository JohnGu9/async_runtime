#pragma once
#include "../ref_implement.h"

template <typename T>
template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type *>
_async_runtime::RefImplement<T>::RefImplement(const ref<R> &other) noexcept : super(other)
{
    DEBUG_ASSERT(super::get() && "ref Uninitiated NullReference Error! As usually this error cause by lateref that use before assgin a non-null reference. ");
}

template <typename T>
template <typename R, typename std::enable_if<std::is_base_of<T, R>::value>::type *>
_async_runtime::RefImplement<T>::RefImplement(ref<R> &&other) noexcept : super(std::move(other))
{
    DEBUG_ASSERT(super::get() && "ref Uninitiated NullReference Error! As usually this error cause by lateref that use before assgin a non-null reference. ");
}

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
