#pragma once
#include "../object/object.h"
#include <cmath>

// https://en.cppreference.com/w/cpp/language/types

class Number : public virtual Object
{
public:
    class Type
    {
    public:
        enum Value
        {
            Short,
            Integer,
            Long,
            LongLong,
            Float,
            Double,
        };
    };

    virtual Number::Type::Value type() = 0;
    template <typename T>
    class Basic;

    class Short;    // at least 16 bits
    class Integer;  // at least 16 bits
    class Long;     // at least 32 bits
    class LongLong; // at least 64 bits

    class Float;  // 32 bits
    class Double; // 64 bits
};

template <typename T>
class Number::Basic : public Number
{
public:
    const T value;
    Basic(T value) : value(value) {}

    size_t hashCode() override
    {
        return std::hash<T>()(value);
    }
    void toStringStream(std::ostream &os) override
    {
        os << value;
    }
    bool operator==(ref<Object> other) override;
};

class Number::Short : public Number::Basic<short>
{
public:
    using Number::Basic<short>::Basic;
    Number::Type::Value type() override; // Number::Type::Short
};

class Number::Integer : public Number::Basic<int>
{
public:
    using Number::Basic<int>::Basic;
    Number::Type::Value type() override; // Number::Type::Integer
};

class Number::Long : public Number::Basic<long>
{
public:
    using Number::Basic<long>::Basic;
    Number::Type::Value type() override; // Number::Type::Long
};

class Number::LongLong : public Number::Basic<long long>
{
public:
    using Number::Basic<long long>::Basic;
    Number::Type::Value type() override; // Number::Type::LongLong
};

class Number::Float : public Number::Basic<float>
{
public:
    using Number::Basic<float>::Basic;
    Number::Type::Value type() override; // Number::Type::Float
};

class Number::Double : public Number::Basic<double>
{
public:
    using Number::Basic<double>::Basic;
    Number::Type::Value type() override; // Number::Type::Double
};

template <typename T>
bool Number::Basic<T>::operator==(ref<Object> other)
{
    if (auto ptr = dynamic_cast<Number *>(other.get()))
    {
        switch (ptr->type())
        {
        case Number::Type::Short:
            return this->value == dynamic_cast<Number::Short *>(ptr)->value;
        case Number::Type::Integer:
            return this->value == dynamic_cast<Number::Integer *>(ptr)->value;
        case Number::Type::Long:
            return this->value == dynamic_cast<Number::Long *>(ptr)->value;
        case Number::Type::LongLong:
            return this->value == dynamic_cast<Number::LongLong *>(ptr)->value;
        case Number::Type::Float:
            return this->value == dynamic_cast<Number::Float *>(ptr)->value;
        case Number::Type::Double:
            return this->value == dynamic_cast<Number::Double *>(ptr)->value;
        default:
            break;
        }
    }
    return false;
}

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

    ref(short value) : super(Object::create<Number::Short>(value)) {}
    ref(int value) : super(Object::create<Number::Integer>(value)) {}
    ref(long value) : super(Object::create<Number::Long>(value)) {}
    ref(long long value) : super(Object::create<Number::LongLong>(value)) {}

    ref(float value) : super(Object::create<Number::Float>(value)) {}
    ref(double value) : super(Object::create<Number::Double>(value)) {}

protected:
    ref() noexcept : super() {}
    using _async_runtime::RefImplement<Number>::RefImplement;
};
