#pragma once

#include "number_signed_basic.h"

template <typename T>
class Number::DoubleBasic : public Number::SignedBasic<T>
{
public:
    using Number::SignedBasic<T>::SignedBasic;
    using Number::SignedBasic<T>::operator==;
    using Number::SignedBasic<T>::operator<;
    using Number::SignedBasic<T>::operator>;

    bool operator==(const size_t &other) override { return this->value == other; }
    bool operator<(const size_t &other) override { return this->value < other; }
    bool operator>(const size_t &other) override { return this->value > other; }
    size_t hashCode() override;
};
