#pragma once

#include "number_signed_basic.h"

template <typename T>
class Number::IntegerBasic : public Number::SignedBasic<T>
{
public:
    using Number::SignedBasic<T>::SignedBasic;
    using Number::SignedBasic<T>::operator==;
    using Number::SignedBasic<T>::operator<;
    using Number::SignedBasic<T>::operator>;

    bool operator==(const size_t &other) override { return Number::cmp<>(this->value, other) == 0; }
    bool operator<(const size_t &other) override { return Number::cmp<>(this->value, other) < 0; }
    bool operator>(const size_t &other) override { return Number::cmp<>(this->value, other) > 0; }
    size_t hashCode() override;
};
