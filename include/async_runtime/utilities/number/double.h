#pragma once
#include "number_double_basic.h"

class Number::Float : public Number::DoubleBasic<float>
{
public:
    using Number::DoubleBasic<float>::DoubleBasic;
    Number::Type::Value type() override; // Number::Type::Float
};

class Number::Double : public Number::DoubleBasic<double>
{
public:
    using Number::DoubleBasic<double>::DoubleBasic;
    Number::Type::Value type() override; // Number::Type::Double
};

class Number::LongDouble : public Number::DoubleBasic<long double>
{
public:
    using Number::DoubleBasic<long double>::DoubleBasic;
    Number::Type::Value type() override; // Number::Type::LongDouble
};
