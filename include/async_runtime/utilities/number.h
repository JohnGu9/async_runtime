#pragma once
#include "../object/object.h"
#include <cmath>

class Number : public Object
{
public:
    class Type
    {
    public:
        enum Value
        {
            Integer,
            Double,
        };
    };

    virtual int toInt() = 0;
    virtual double toDouble() = 0;
    virtual Number::Type::Value type() = 0;

    class Integer;
    class Double;
};

class Number::Integer : public Number
{
public:
    const int value;
    Integer(int value) : value(value) {}

    int toInt() override;
    double toDouble() override;
    Number::Type::Value type() override; // Number::Type::Integer

    bool operator==(ref<Object> other) override;
    size_t hashCode() override;
    void toStringStream(std::ostream &os) override;
};

class Number::Double : public Number
{
public:
    const double value;
    Double(double value) : value(value) {}

    int toInt() override;
    double toDouble() override;
    Number::Type::Value type() override; // Number::Type::Double

    bool operator==(ref<Object> other) override;
    size_t hashCode() override;
    void toStringStream(std::ostream &os) override;
};

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

    ref(int value) : super(Object::create<Number::Integer>(value)) {}
    ref(double value) : super(Object::create<Number::Double>(value)) {}

protected:
    ref() noexcept : super() {}
    using _async_runtime::RefImplement<Number>::RefImplement;
};
