#pragma once
#include "../object/object.h"
#include <cmath>

class Number : public Object
{
public:
    virtual int toInt() = 0;
    virtual double toDouble() = 0;
    class Integer;
    class Double;
};

class Number::Integer : public Number
{
public:
    const int value;
    Integer(int value) : value(value) {}

    int toInt() override { return value; }
    double toDouble() override { return value; }

    bool operator==(ref<Object> other) override
    {
        if (auto ptr = dynamic_cast<Integer *>(other.get()))
        {
            return this->value == ptr->value;
        }
        return false;
    }

    size_t hashCode() override
    {
        return std::hash<int>()(value);
    }

    void toStringStream(std::ostream &os) override { os << value; }
};

class Number::Double : public Number
{
public:
    const double value;
    Double(double value) : value(value) {}

    int toInt() override { return static_cast<int>(value); }
    double toDouble() override { return value; }

    bool operator==(ref<Object> other) override
    {
        if (auto ptr = dynamic_cast<Double *>(other.get()))
        {
            return this->value == ptr->value;
        }
        return false;
    }

    size_t hashCode() override
    {
        return std::hash<double>()(value);
    }

    void toStringStream(std::ostream &os) override { os << value; }
};

template <>
class ref<Number> : public _async_runtime::RefImplement<Number>
{
    _ASYNC_RUNTIME_FRIEND_FAMILY;
    using super = _async_runtime::RefImplement<Number>;

public:
    template <typename R, typename std::enable_if<std::is_base_of<Number, R>::value>::type * = nullptr>
    ref(const ref<R> &other) : super(other) {}
    template <typename R, typename std::enable_if<std::is_base_of<Number, R>::value>::type * = nullptr>
    ref(ref<R> &&other) : super(std::move(other)) {}

    ref(int value) : super(Object::create<Number::Integer>(value)) {}
    ref(double value) : super(Object::create<Number::Double>(value)) {}

protected:
    ref() {}
    using _async_runtime::RefImplement<Number>::RefImplement;
};
