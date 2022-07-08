#pragma once

#include "number_double_basic.h"
#include <cmath>

template <typename T>
size_t Number::DoubleBasic<T>::hashCode()
{
    auto f = static_cast<size_t>(std::floor(this->value));
    if (f == this->value)
        return f;
    return std::hash<T>()(this->value);
}
