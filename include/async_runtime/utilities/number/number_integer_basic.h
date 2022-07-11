#pragma once

#include "number_signed_basic.h"

template <typename T>
class Number::IntegerBasic : public Number::SignedBasic<T>
{
    using super = Number::SignedBasic<T>;

public:
    using super::super;
    using super::operator==;
    using super::operator<;
    using super::operator>;

    bool operator==(const size_t &other) override { return Number::cmp<>(this->value, other) == 0; }
    bool operator<(const size_t &other) override { return Number::cmp<>(this->value, other) < 0; }
    bool operator>(const size_t &other) override { return Number::cmp<>(this->value, other) > 0; }
    size_t hashCode() override { return static_cast<size_t>(this->value); }
};
