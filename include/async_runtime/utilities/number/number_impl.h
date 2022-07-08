#pragma once

#include "number.h"

template <typename T>
int Number::cmp(const size_t &s, const T &other)
{
    if (other < 0)
        return 1;

    auto unsignedNumber = static_cast<unsigned long long>(other);
    if (s < unsignedNumber)
        return -1;
    else if (s > unsignedNumber)
        return 1;
    else
        return 0;
}

template <typename T>
int Number::cmp(const T &other, const size_t &s)
{
    if (other < 0)
        return -1;

    auto unsignedNumber = static_cast<unsigned long long>(other);
    if (s < unsignedNumber)
        return -1;
    else if (s > unsignedNumber)
        return 1;
    else
        return 0;
}
