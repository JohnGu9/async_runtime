#pragma once

#include "number_integer_basic.h"
#include <cmath>

template <typename T>
size_t Number::IntegerBasic<T>::hashCode()
{
    return static_cast<size_t>(std::abs(this->value));
}
