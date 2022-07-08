#pragma once

#include "double.h"
#include "integer.h"

template <>
class ref<Number> : public _async_runtime::RefImplement<Number>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Number>;

public:
    template <typename R, typename std::enable_if<std::is_base_of<Number, R>::value>::type * = nullptr>
    ref(const ref<R> &other) noexcept : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<Number, R>::value>::type * = nullptr>
    ref(ref<R> &&other) noexcept : super(std::move(other)) {}

    ref(const size_t &value) : super(Object::create<Number::SizeType>(value)) {}

    ref(const short &value) : super(Object::create<Number::Short>(value)) {}
    ref(const int &value) : super(Object::create<Number::Integer>(value)) {}
    ref(const long &value) : super(Object::create<Number::Long>(value)) {}
    ref(const long long &value) : super(Object::create<Number::LongLong>(value)) {}

    ref(const float &value) : super(Object::create<Number::Float>(value)) {}
    ref(const double &value) : super(Object::create<Number::Double>(value)) {}
    ref(const long double &value) : super(Object::create<Number::LongDouble>(value)) {}

protected:
    ref() noexcept : super() {}
    using _async_runtime::RefImplement<Number>::RefImplement;
};
