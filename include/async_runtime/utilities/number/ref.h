#pragma once
#include "double.h"
#include "integer.h"
#include "size_type.h"

template <>
class ref<Number> : public _async_runtime::RefImplement<Number>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Number>;

public:
    using super::super;

    ref(const size_t &value) : super(Object::create<Number::SizeType>(value)) {}

    ref(const short &value) : super(Object::create<Number::Short>(value)) {}
    ref(const int &value) : super(Object::create<Number::Integer>(value)) {}
    ref(const long &value) : super(Object::create<Number::Long>(value)) {}
    ref(const long long &value) : super(Object::create<Number::LongLong>(value)) {}

    ref(const float &value) : super(Object::create<Number::Float>(value)) {}
    ref(const double &value) : super(Object::create<Number::Double>(value)) {}
    ref(const long double &value) : super(Object::create<Number::LongDouble>(value)) {}

protected:
    ref() noexcept = default;
};

ref<Number> operator""_Number(unsigned long long int);
ref<Number> operator""_Number(long double);
