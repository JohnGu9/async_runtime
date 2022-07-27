#pragma once
#include "number_basic.h"

class Number::SizeType : public Number::Basic<size_t>
{
    using super = Number::Basic<size_t>;

public:
    using super::super;
    using super::operator==;
    using super::operator<;
    using super::operator>;

    Number::Type::Value type() override; // Number::Type::SizeType
    size_t hashCode() override;

    bool operator==(const size_t &) override;
    bool operator==(const short &) override;
    bool operator==(const int &) override;
    bool operator==(const long &) override;
    bool operator==(const long long &) override;

    bool operator<(const size_t &) override;
    bool operator<(const short &) override;
    bool operator<(const int &) override;
    bool operator<(const long &) override;
    bool operator<(const long long &) override;

    bool operator>(const size_t &) override;
    bool operator>(const short &) override;
    bool operator>(const int &) override;
    bool operator>(const long &) override;
    bool operator>(const long long &) override;
};
