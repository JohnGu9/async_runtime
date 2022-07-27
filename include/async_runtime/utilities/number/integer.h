#pragma once
#include "number_integer_basic.h"

class Number::Short : public Number::IntegerBasic<short>
{
public:
    using Number::IntegerBasic<short>::IntegerBasic;
    Number::Type::Value type() override; // Number::Type::Short
};

class Number::Integer : public Number::IntegerBasic<int>
{
public:
    using Number::IntegerBasic<int>::IntegerBasic;
    Number::Type::Value type() override; // Number::Type::Integer
};

class Number::Long : public Number::IntegerBasic<long>
{
public:
    using Number::IntegerBasic<long>::IntegerBasic;
    Number::Type::Value type() override; // Number::Type::Long
};

class Number::LongLong : public Number::IntegerBasic<long long>
{
public:
    using Number::IntegerBasic<long long>::IntegerBasic;
    Number::Type::Value type() override; // Number::Type::LongLong
};
