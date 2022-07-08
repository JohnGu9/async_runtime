#pragma once

#include "number_basic.h"

class Number::SizeType : public Number::Basic<size_t>
{
public:
    using Number::Basic<size_t>::Basic;
    using Number::Basic<size_t>::operator==;
    using Number::Basic<size_t>::operator<;
    using Number::Basic<size_t>::operator>;

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
