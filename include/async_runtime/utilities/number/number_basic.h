#pragma once

#include "number.h"

template <typename T>
class Number::Basic : public Number
{
    using super = Number;

public:
    const T value;
    Basic(const T &value) : value(value) {}

    void toStringStream(std::ostream &os) override { os << value; }
    bool operator==(const option<Object> &) override;

    bool operator<(const ref<Number> &) override;
    bool operator>(const ref<Number> &) override;
    bool operator<(const option<Number> &) override;
    bool operator>(const option<Number> &) override;

    bool operator==(const float &other) override { return this->value == other; }
    bool operator==(const double &other) override { return this->value == other; }
    bool operator==(const long double &other) override { return this->value == other; }

    bool operator<(const float &other) override { return this->value < other; }
    bool operator<(const double &other) override { return this->value < other; }
    bool operator<(const long double &other) override { return this->value < other; }

    bool operator>(const float &other) override { return this->value > other; }
    bool operator>(const double &other) override { return this->value > other; }
    bool operator>(const long double &other) override { return this->value > other; }

    using super::operator==;
    using super::operator<;
    using super::operator>;
};
