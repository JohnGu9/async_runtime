#pragma once

#include "number.h"

template <typename T>
class Number::Basic : public Number
{
public:
    const T value;
    Basic(const T &value) : value(value) {}

    void toStringStream(std::ostream &os) override { os << value; }
    bool operator==(const option<Object> &) override;

    bool operator<(const ref<Number> &) override;
    bool operator>(const ref<Number> &) override;
    bool operator<(const option<Number> &) override;
    bool operator>(const option<Number> &) override;

    using Number::operator==;
    using Number::operator<;
    using Number::operator>;
};
