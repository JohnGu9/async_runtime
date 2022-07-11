#pragma once

#include "number_signed_basic.h"

template <typename T>
class Number::DoubleBasic : public Number::SignedBasic<T>
{
    using super = Number::SignedBasic<T>;

public:
    using super::super;
    using super::operator==;
    using super::operator<;
    using super::operator>;

    bool operator==(const size_t &other) override { return this->value == other; }
    bool operator<(const size_t &other) override { return this->value < other; }
    bool operator>(const size_t &other) override { return this->value > other; }
    size_t hashCode() override
    {
        auto f = static_cast<size_t>(std::floor(this->value));
        if (f == this->value) // [[unlikely]]
            return f;
        return std::hash<long double>()(this->value);
    }
};
