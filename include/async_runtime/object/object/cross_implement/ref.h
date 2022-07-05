#pragma once

#include "../ref_implement.h"

template <typename T>
template <typename OTHER>
bool _async_runtime::RefImplement<T>::operator==(const OTHER &other) const
{
    return this->get()->operator==(other);
}

template <typename T>
template <typename OTHER>
bool _async_runtime::RefImplement<T>::operator<(const OTHER &other) const
{
    return this->get()->operator<(other);
}

template <typename T>
template <typename OTHER>
bool _async_runtime::RefImplement<T>::operator>(const OTHER &other) const
{
    return this->get()->operator>(other);
}
