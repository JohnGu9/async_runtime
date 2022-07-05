#pragma once

#include "cross_implement/else.h"
#include "cross_implement/object.h"
#include "cross_implement/option.h"
#include "cross_implement/weakref.h"

template <typename T>
template <typename OTHER>
bool _async_runtime::RefImplement<T>::operator==(const OTHER &other) const
{
    return this->get()->operator==(other);
}

template <typename T>
template <typename OTHER, typename std::enable_if<_async_runtime::OptionFilter<OTHER>::value>::type *>
bool option<T>::operator==(const OTHER &other) const
{
    if (this->get() == nullptr)
        return false;
    return this->super::operator==(other);
}

template <typename T>
template <typename R>
bool option<T>::operator==(const option<R> &other) const
{
    if (this->get() != nullptr)
        return this->super::operator==(other);
    else if (other.get() != nullptr)
        return (&other)->ref<R>::operator==(*this);
    return true;
}
