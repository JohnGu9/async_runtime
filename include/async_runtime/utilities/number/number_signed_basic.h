#pragma once
#include "number_basic.h"

template <typename T>
class Number::SignedBasic : public Number::Basic<T>
{
    using super = Number::Basic<T>;

public:
    using super::super;
    using super::operator==;
    using super::operator<;
    using super::operator>;

    bool operator==(const short &other) override { return this->value == other; }
    bool operator==(const int &other) override { return this->value == other; }
    bool operator==(const long &other) override { return this->value == other; }
    bool operator==(const long long &other) override { return this->value == other; }

    bool operator<(const short &other) override { return this->value < other; }
    bool operator<(const int &other) override { return this->value < other; }
    bool operator<(const long &other) override { return this->value < other; }
    bool operator<(const long long &other) override { return this->value < other; }

    bool operator>(const short &other) override { return this->value > other; }
    bool operator>(const int &other) override { return this->value > other; }
    bool operator>(const long &other) override { return this->value > other; }
    bool operator>(const long long &other) override { return this->value > other; }
};
